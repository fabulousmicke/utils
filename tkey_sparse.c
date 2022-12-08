/**
 * @file tkey_sparse.c
 * @brief Ternary sparse key implementation.
 * @author Mikael Sundstrom <micke@fabinv.com>
 *
 * @copyright Copyright (c) 2020 Fabulous Inventions AB - all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#include <tquad.h>
#include <cfix.h>
#include <m2.h>

#include <tkey_sparse.h>

#define T fprintf(stderr, "\nFILE %s, LINE %d\n", __FILE__, __LINE__);

#define TKEY_SPARSE_PAIR_COUNT (1 << 16)

struct tkey_sparse {
	ttype_t *type;				/**< Ternary key type. */
	cfix_t *cfix;					/**< CFIX instance (64 bits). */
	size_t bits;					/**< Ternary key size (64 bits). */
	unsigned long quads : 61;
	tbase_t base : 1;			/**< Ternary key base binary or ternary (8 bits). */
	tbit_t dflt : 2;				/**< Ternary key default bit value (8 bits). */
};

struct tkey_sparse_hist {
	ttype_t *type;
	cfix_t *cfix0;
	cfix_t *cfix1;
	size_t count;
	tbit_t dflt;
};

struct tkey_sparse_pair {
	tquad_t quad;
	uint32_t index;
};
typedef struct tkey_sparse_pair tkey_sparse_pair_t;

static m2_t *tkey_sparse_handle = NULL, *tkey_sparse_hist_handle = NULL;

static cfix_config_t tkey_sparse_cfix_config = {
	1,
	2,
	3,
	0.35,
	0.95,
	CFIX_CONFIG_DEFAULT_GROWTH,
	CFIX_CONFIG_DEFAULT_ATTEMPT,
	CFIX_CONFIG_DEFAULT_RANDOM      
};

static cfix_config_t tkey_sparse_hist_cfix_config = {
	10,
	1,
	3,
	0.0,
	0.95,
	CFIX_CONFIG_DEFAULT_GROWTH,
	CFIX_CONFIG_DEFAULT_ATTEMPT,
	CFIX_CONFIG_DEFAULT_RANDOM      
};

	static tkey_sparse_t *
tkey_sparse_reuse(void)
{
	if (tkey_sparse_handle == NULL) {
		tkey_sparse_handle = m2_create("tkey_sparse_t", sizeof(tkey_sparse_t));
	}
	return (tkey_sparse_t *)m2_reuse(tkey_sparse_handle, 1, false);
}

	static void
tkey_sparse_recycle(tkey_sparse_t *key)
{
	m2_recycle(tkey_sparse_handle, (void *)key, 1);
}

	static tkey_sparse_hist_t *
tkey_sparse_hist_reuse(void)
{
	if (tkey_sparse_hist_handle == NULL) {
		tkey_sparse_hist_handle = m2_create("tkey_sparse_hist_t", sizeof(tkey_sparse_hist_t));
	}
	return (tkey_sparse_hist_t *)m2_reuse(tkey_sparse_hist_handle, 1, false);
}

	static void
tkey_sparse_hist_recycle(tkey_sparse_hist_t *hist)
{
	m2_recycle(tkey_sparse_hist_handle, (void *)hist, 1);
}

	static inline size_t
tkey_sparse_quads(
		size_t bits)
{
	size_t result;

	result = bits / TQUAD_BITS;
	result += ((result * TQUAD_BITS) == bits) ? 0 : 1;

	return result;
}

	void
tkey_sparse_create(
		tkey_sparse_t **key,
		ttype_t *type)
{
	//	fprintf(stderr, "sizeof(tkey_sparse_t) = %zu, TKEY_SPARSE_INTERNAL_SIZE = %zu, TKEY_SPARSE_EXTERNAL_SIZE = %zu",
	//			sizeof(tkey_sparse_t), TKEY_SPARSE_INTERNAL_SIZE, TKEY_SPARSE_EXTERNAL_SIZE);
	//	getchar();
	(*key) = tkey_sparse_reuse();
	assert((*key) != NULL);

	(*key)->type = type;
	(*key)->dflt = ttype_default(type);
	(*key)->base = ttype_base(type);
	(*key)->bits = ttype_size(type);
	(*key)->quads = (unsigned)tkey_sparse_quads((*key)->bits);
	cfix_create(&(*key)->cfix, &tkey_sparse_cfix_config);
}	

	void
tkey_sparse_destroy(tkey_sparse_t **key)
{
	cfix_destroy(&(*key)->cfix);
	tkey_sparse_recycle((*key));
	(*key) = NULL;
}

	ttype_t *
tkey_sparse_type(
		tkey_sparse_t *key)
{
	return key->type;
}

	void
tkey_sparse_clone(
		tkey_sparse_t *original,
		tkey_sparse_t **clone)
{
	size_t index;

	tkey_sparse_create(clone, original->type);
	for (index = 0; index < original->bits; index++) {
		tkey_sparse_put((*clone), index, tkey_sparse_get(original, index));
	}
}

#if 0
	static int
tkey_sparse_compare(
		const void *p1,
		const void *p2)
{
	uint32_t index1 = ((tkey_sparse_pair_t *)p1)->index,
			 index2 = ((tkey_sparse_pair_t *)p2)->index;

	if (index1 < index2) return -1;
	if (index1 > index2) return 1;
	return 0;
}
#endif

	void
tkey_sparse_preamble(
		tkey_sparse_t *key,
		char *pre)
{
	pre[1] = 'U';
	pre[2] = 'S';
	pre[3] = '_';
	//	pre[3] = (key->base == TKEY_SPARSE_BASE_BINARY) ? 'B' : 'T';
	pre[4] = tbit_to_char(key->dflt);
}

	size_t
tkey_sparse_from_string(
		tkey_sparse_t *key,
		char *buf,
		size_t len)
{
	size_t index, max_index;
	tbit_t bit;

	if (key->bits < len) {
		max_index = key->bits;
	} else {
		max_index = len;
	}

	assert(key != NULL);
	for (index = 0; (index < max_index) && tbit_from_char(buf[index], &bit); index++) {
		tkey_sparse_put(key, index, bit);
	}

	return index;
}

	void
tkey_sparse_to_string(
		tkey_sparse_t *key,
		char *buf, 
		size_t len)
{
	size_t index, next;

	next = 0;
	for (index = 0; index < key->bits; index++) {
		if (next == len - 1) break;
		buf[next++] = tbit_to_char(tkey_sparse_get(key, index));
	}
	buf[next] = '\0';
}

	static inline void
tkey_sparse_index(
		size_t index,
		size_t *quad_index,
		size_t *bit_index)
{
	(*quad_index) = index / TQUAD_BITS;
	(*bit_index) = index % TQUAD_BITS;
}

	static inline tquad_t
tkey_sparse_default(
		tbit_t bit)
{
	static uint32_t bits[3] = {0x00000000, 0xffffffff, 0x00000000},
					mask[3] = {0xffffffff, 0xffffffff, 0x00000000};

	return tquad(bits[bit], mask[bit]);
}

	tbit_t
tkey_sparse_get(
		tkey_sparse_t *key,
		size_t index)
{
	tquad_t quad;
	size_t quad_index, bit_index;

	tkey_sparse_index(index, &quad_index, &bit_index);
	if (cfix_lookup(key->cfix, quad_index, (uint32_t *)&quad)) {
		return tquad_get(quad, bit_index);
	}
	return key->dflt;
}

	void
tkey_sparse_put(
		tkey_sparse_t *key,
		size_t index,
		tbit_t bit)
{
	tquad_t quad, quad_dflt;
	size_t quad_index, bit_index;

	quad_dflt = tkey_sparse_default(key->dflt);

	tkey_sparse_index(index, &quad_index, &bit_index);
	if (!cfix_lookup(key->cfix, quad_index, (uint32_t *)&quad)) {
		if (bit == key->dflt) {
			/* Bit is already assigned target value - do nothing! */
			return;
		}
		quad = quad_dflt;
		cfix_insert(key->cfix, quad_index, (uint32_t *)&quad);
	}

	if (bit == tquad_get(quad, bit_index)) {
		/* Bit is already assigned target value - do nothing! */
		return;
	}

	tquad_put(&quad, bit_index, bit);

	if (quad == quad_dflt) {
		/*
		 * Default chunks does not need to be explicitly stored.
		 */
		cfix_delete(key->cfix, quad_index);
		return;
	}	

	cfix_update(key->cfix, quad_index, (uint32_t *)&quad);
}

	bool
tkey_sparse_quad(tkey_sparse_t *key, size_t quad_index, uint64_t *quad)
{
	if (key == NULL) return false;
	if (quad_index >= key->quads) return false;

	if (!cfix_lookup(key->cfix, quad_index, (uint32_t *)quad)) {
		(*quad) = tkey_sparse_default(key->dflt);
	}
	return true;
}

	relation_t
tkey_sparse_relation(
		tkey_sparse_t *key1,
		tkey_sparse_t *key2)
{
	cfix_iter_t *iter;
	tquad_t quad1, quad2, quad_dflt1, quad_dflt2;
	size_t quad_count1, quad_count2, quad_limit, expl_expl, expl_dflt, dflt_expl, dflt_dflt;
	uint32_t quad_index;
	relation_t result, current;

	if (key1 == key2) return RELATION_EQUAL;

	assert(key1->type == key2->type);

	quad_count1 = cfix_keys(key1->cfix);
	quad_count2 = cfix_keys(key2->cfix);

	if (quad_count1 == 0 && quad_count2 == 0) {
		if (key1->dflt == key2->dflt) {
			result = RELATION_EQUAL;
		} else if (key1->dflt == TBIT_) {
			result = RELATION_SUPERSET;
		} else if (key2->dflt == TBIT_) {
			result = RELATION_SUBSET;
		} else {
			result = RELATION_DISJOINT;
		}
		return result;
	}

	result = RELATION_EQUAL;
	quad_dflt1 = tkey_sparse_default(key1->dflt);
	quad_dflt2 = tkey_sparse_default(key2->dflt);
	quad_limit = (size_t)key1->quads;

	if (quad_count1 == 0) {
		quad1 = quad_dflt1;

		if (quad_count2 < quad_limit) {
			current = tquad_relation(quad1, quad_dflt2);
			relation_update(&result, current);
			if (result == RELATION_DISJOINT) return result;
		}

		cfix_iter_create(key2->cfix, &iter);
		do {
			assert(cfix_iter_current(key2->cfix, iter, &quad_index, (uint32_t *)&quad2) == CFIX_ITER_SUCCESS);

			current = tquad_relation(quad1, quad2);
			relation_update(&result, current);
			if (result == RELATION_DISJOINT) break;
		} while (cfix_iter_forward(key2->cfix, iter) == CFIX_ITER_SUCCESS);
		cfix_iter_destroy(&iter);

		return result;
	}

	if (quad_count2 == 0) {
		quad2 = quad_dflt2;

		if (quad_count1 < quad_limit) {
			current = tquad_relation(quad_dflt1, quad2);
			relation_update(&result, current);
			if (result == RELATION_DISJOINT) return result;
		}

		cfix_iter_create(key1->cfix, &iter);
		do {
			assert(cfix_iter_current(key1->cfix, iter, &quad_index, (uint32_t *)&quad1) == CFIX_ITER_SUCCESS);

			current = tquad_relation(quad1, quad2);
			relation_update(&result, current);
			if (result == RELATION_DISJOINT) break;
		} while (cfix_iter_forward(key1->cfix, iter) == CFIX_ITER_SUCCESS);
		cfix_iter_destroy(&iter);

		return result;
	}

	expl_expl = expl_dflt = dflt_expl = 0;

	cfix_iter_create(key1->cfix, &iter);
	do {
		assert(cfix_iter_current(key1->cfix, iter, &quad_index, (uint32_t *)&quad1) == CFIX_ITER_SUCCESS);

		if (cfix_lookup(key2->cfix, quad_index, (uint32_t *)&quad2)) {
			current = tquad_relation(quad1, quad2);
			++expl_expl;
		} else {
			current = tquad_relation(quad1, quad_dflt2);
			++expl_dflt;
		}
		relation_update(&result, current);
		if (result == RELATION_DISJOINT) break;
	} while (cfix_iter_forward(key1->cfix, iter) == CFIX_ITER_SUCCESS);
	cfix_iter_destroy(&iter);

	if (result == RELATION_DISJOINT) return result;

	cfix_iter_create(key2->cfix, &iter);
	do {
		assert(cfix_iter_current(key2->cfix, iter, &quad_index, (uint32_t *)&quad2) == CFIX_ITER_SUCCESS);

		if (cfix_lookup(key1->cfix, quad_index, (uint32_t *)&quad1)) {
			/*
			 * Already compared.
			 */
		} else {
			current = tquad_relation(quad_dflt1, quad2);
			++dflt_expl;
		}
		relation_update(&result, current);
		if (result == RELATION_DISJOINT) break;
	} while (cfix_iter_forward(key2->cfix, iter) == CFIX_ITER_SUCCESS);
	cfix_iter_destroy(&iter);

	if (result == RELATION_DISJOINT) return result;

	dflt_dflt = quad_limit - (expl_expl + expl_dflt + dflt_expl);
	if (dflt_dflt > 0) {
		current = tquad_relation(quad_dflt1, quad_dflt2);
		relation_update(&result, current);
	}

	return result;
}

	void
tkey_sparse_hist_create(
		tkey_sparse_hist_t **hist,
		ttype_t *type)
{
	(*hist) = tkey_sparse_hist_reuse();
	(*hist)->type = type;
	cfix_create(&(*hist)->cfix0, &tkey_sparse_hist_cfix_config);
	cfix_create(&(*hist)->cfix1, &tkey_sparse_hist_cfix_config);
	(*hist)->count = 0;
	(*hist)->dflt = ttype_default(type);;
}

	void
tkey_sparse_hist_destroy(tkey_sparse_hist_t **hist)
{
	cfix_destroy(&(*hist)->cfix0);
	cfix_destroy(&(*hist)->cfix1);
	tkey_sparse_hist_recycle((*hist));
	(*hist) = NULL;
}

	ttype_t *
tkey_sparse_hist_type(
		tkey_sparse_hist_t *hist)
{
	return hist->type;
}

	void
tkey_sparse_hist_add(
		tkey_sparse_hist_t *hist,
		tkey_sparse_t *key)
{
	cfix_t *cfix[3];
	cfix_iter_t *iter;
	tquad_t quad;
	size_t bit_index;
	uint32_t quad_index, index, count;
	tbit_t bit;

	assert(key->type == hist->type);

	++hist->count;
	if (cfix_keys(key->cfix) == 0) return;

	switch (hist->dflt) {
		case TBIT0:
			cfix[TBIT0] = NULL;
			cfix[TBIT1] = hist->cfix1;
			cfix[TBIT_] = hist->cfix0;
			break;
		case TBIT1:
			cfix[TBIT0] = hist->cfix0;
			cfix[TBIT1] = NULL;
			cfix[TBIT_] = hist->cfix1;
			break;
		case TBIT_:
			cfix[TBIT0] = hist->cfix0;
			cfix[TBIT1] = hist->cfix1;
			cfix[TBIT_] = NULL;
			break;
		default:
			/* *** NOT REACHED *** */
			assert(0);
	}
	cfix_iter_create(key->cfix, &iter);
	do {
		assert(cfix_iter_current(key->cfix, iter, &quad_index, (uint32_t *)&quad) == CFIX_ITER_SUCCESS);
		index = quad_index * TQUAD_BITS;

		for (bit_index = 0; bit_index < TQUAD_BITS; bit_index++, index++) {
			bit = tquad_get(quad, bit_index);
			if (bit != hist->dflt) {
				if (cfix_lookup(cfix[bit], index, &count)) {
					++count;
					cfix_update(cfix[bit], index, &count);
				} else {
					count = 1;
					cfix_insert(cfix[bit], index, &count);
				}
			}
		}
	} while (cfix_iter_forward(key->cfix, iter) == CFIX_ITER_SUCCESS);
	cfix_iter_destroy(&iter);
}

	void
tkey_sparse_hist_del(
		tkey_sparse_hist_t *hist,
		tkey_sparse_t *key)
{
	cfix_t *cfix[3];
	cfix_iter_t *iter;
	tquad_t quad;
	size_t bit_index;
	uint32_t quad_index, index, count;
	tbit_t bit;

	assert(key->type == hist->type);

	--hist->count;
	if (cfix_keys(key->cfix) == 0) return;

	switch (hist->dflt) {
		case TBIT0:
			cfix[TBIT0] = NULL;
			cfix[TBIT1] = hist->cfix1;
			cfix[TBIT_] = hist->cfix0;
			break;
		case TBIT1:
			cfix[TBIT0] = hist->cfix0;
			cfix[TBIT1] = NULL;
			cfix[TBIT_] = hist->cfix1;
			break;
		case TBIT_:
			cfix[TBIT0] = hist->cfix0;
			cfix[TBIT1] = hist->cfix1;
			cfix[TBIT_] = NULL;
			break;
		default:
			/* *** NOT REACHED *** */
			assert(0);
	}
	cfix_iter_create(key->cfix, &iter);
	do {
		assert(cfix_iter_current(key->cfix, iter, &quad_index, (uint32_t *)&quad) == CFIX_ITER_SUCCESS);
		index = quad_index * TQUAD_BITS;

		for (bit_index = 0; bit_index < TQUAD_BITS; bit_index++, index++) {
			bit = tquad_get(quad, bit_index);
			if (bit != hist->dflt) {
				assert(cfix_lookup(cfix[bit], index, &count));
				--count;
				if (count == 0) {
					cfix_delete(cfix[bit], index);
				} else {
					cfix_update(cfix[bit], index, &count);
				}
			}
		}
	} while (cfix_iter_forward(key->cfix, iter) == CFIX_ITER_SUCCESS);
	cfix_iter_destroy(&iter);
}

	size_t
tkey_sparse_hist_count(tkey_sparse_hist_t *hist)
{
	return hist->count;
}

	void
tkey_sparse_hist_dist(
		tkey_sparse_hist_t *hist,
		size_t index,
		size_t *dist)
{
	uint32_t count0, count1;

	if (!cfix_lookup(hist->cfix0, (uint32_t)index, &count0)) count0 = 0;
	if (!cfix_lookup(hist->cfix1, (uint32_t)index, &count1)) count1 = 0;

	switch (hist->dflt) {
		case TBIT0:
			dist[TBIT0] = hist->count - (count0 + count1);
			dist[TBIT1] = count1;
			dist[TBIT_] = count0;
			break;
		case TBIT1:
			dist[TBIT0] = count0;
			dist[TBIT1] = hist->count - (count0 + count1);;
			dist[TBIT_] = count1;
			break;
		case TBIT_:
			dist[TBIT0] = count0;
			dist[TBIT1] = count1;
			dist[TBIT_] = hist->count - (count0 + count1);;
			break;
		default:
			/* *** NOT REACHED *** */
			assert(0);
	}
}

	static inline void
tkey_sparse_hist_map(
		tbit_t dflt,
		size_t c0,
		size_t c1,
		size_t n,
		size_t *n0,
		size_t *n1,
		size_t *n_)
{
	switch (dflt) {
		case TBIT0:
			(*n0) = n - (c0 + c1);
			(*n1) = c1;
			(*n_) = c0;
			break;
		case TBIT1:
			(*n0) = c0;
			(*n1) = n - (c0 + c1);
			(*n_) = c1;
			break;
		case TBIT_:
			(*n0) = c0;
			(*n1) = c1;
			(*n_) = n - (c0 + c1);
			break;
		default:
			/* *** NOT REACHED *** */
			assert(0);
	}
}

	static inline double
tkey_sparse_hist_cost(
		size_t n,
		size_t n0,
		size_t n1,
		size_t n_)
{	
	float l = (double)n0,
		  r = (double)n1,
		  b = (double)n_,
		  n2 = (double)(n * n);

	return b * b - l * r + 1 / (l * r + 1 / n2);
}

	bool
tkey_sparse_hist_disc(
		tkey_sparse_hist_t *hist,
		size_t *index)
{
	cfix_iter_t *iter;
	size_t c0, c1, n0, n1, n_, n, best_n0, best_n1, size, dflt_count;
	double best_cost, curr_cost;
	uint32_t count, curr_index, best_index;
	tbit_t dflt;

	dflt = ttype_default(hist->type);
	size = ttype_size(hist->type);

	best_cost = DBL_MAX;
	best_n0 = best_n1 = 0;
	best_index = size;

	n = dflt_count = hist->count;

	if (cfix_keys(hist->cfix0) > 0) {
		cfix_iter_create(hist->cfix0, &iter);
		do {
			assert(cfix_iter_current(hist->cfix0, iter, &curr_index, &count) == CFIX_ITER_SUCCESS);
			c0 = (size_t)count;

			if (!cfix_lookup(hist->cfix1, curr_index, &count)) count = 0;
			c1 = (size_t)count;

			tkey_sparse_hist_map(dflt, c0, c1, n, &n0, &n1, &n_);

			curr_cost = tkey_sparse_hist_cost(n, n0, n1, n_);

			if ((curr_cost < best_cost) || ((curr_cost == best_cost) && (curr_index < best_index))) {
				best_cost = curr_cost;
				best_index = curr_index;
				best_n0 = n0;
				best_n1 = n1;
			}

			--dflt_count;
		} while (cfix_iter_forward(hist->cfix0, iter) == CFIX_ITER_SUCCESS);
		cfix_iter_destroy(&iter);
	}

	if (cfix_keys(hist->cfix1) > 0) {
		cfix_iter_create(hist->cfix1, &iter);
		do {
			assert(cfix_iter_current(hist->cfix1, iter, &curr_index, &count) == CFIX_ITER_SUCCESS);
			c1 = (size_t)count;

			if (cfix_lookup(hist->cfix0, curr_index, &count)) continue;
			c0 = (size_t)count;

			tkey_sparse_hist_map(dflt, c0, c1, n, &n0, &n1, &n_);

			curr_cost = tkey_sparse_hist_cost(n, n0, n1, n_);

			if (curr_cost < best_cost) {
				best_cost = curr_cost;
				best_index = curr_index;
				best_n0 = n0;
				best_n1 = n1;
			}

			--dflt_count;
		} while (cfix_iter_forward(hist->cfix1, iter) == CFIX_ITER_SUCCESS);
		cfix_iter_destroy(&iter);
	}

	if (dflt_count > 0) {
		c0 = c1 = 0;
		tkey_sparse_hist_map(dflt, c0, c1, n, &n0, &n1, &n_);

		curr_cost = tkey_sparse_hist_cost(n, n0, n1, n_);

		if (curr_cost < best_cost) {
			best_cost = curr_cost;
			best_index = curr_index;
			best_n0 = n0;
			best_n1 = n1;
		}
	}

	(*index) = best_index;

	return (best_n0 < n) && (best_n1 < n);
}

