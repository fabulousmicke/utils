/**
 * @file tkey_main.c
 * @brief Ternary key test program.
 * @author Mikael Sundstrom <micke@fabinv.com>
 *
 * @copyright Copyright (c) 2019 Fabulous Inventions AB - all rights reserved.
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
#include <stdlib.h>
#include <stdbool.h>

#include <m2.h>
#include <bench.h>

#include "tkey.h"
//#include "tkey_basic.h"
//#include "tkey_hybrid.h"
//#include "tkey_dense.h"
#include "tkey_sparse.h"
//#include "tkey_ultra.h"
//#include "tkey_block.h"

//#if TKEY_IMPLEMENTATION == TKEY_FIXED_IMPLEMENTATION
//#	define W TKEY_FIXED_BITS
//#elif TKEY_IMPLEMENTATION == TKEY_SPARSE_IMPLEMENTATION
//#	define W (1 << 20)
//#	define W (1 << 6)
//#endif
#ifdef TKEY_FIXED
#define SIZE TKEY_FIXED_SIZE
#else
#define BASE TKEY_BASE_TERNARY
#define SIZE (1 << 10)
#endif
#define DFLT TBIT1

#define VERBOSE
#define HIST_REF

#define KEYS (1 << 8)

tbit_t ref_bit[SIZE];

//static tbit_t simple_bit[W];

#ifdef HIST_REF
static size_t hist0[SIZE], hist1[SIZE], hist_[SIZE];
#endif

#define PLIM 206

	static void
keyprint(char *buf)
{
	int i;

	if (buf == NULL) {
		for (i = 0; i < PLIM; i++) {
			if ((i % 10) == 0) printf(" ");
			else printf("%d", (i / 10) % 10);
		}
		printf("\n");
		for (i = 0; i < PLIM; i++) {
			printf("%d", i % 10);
		}
		printf("\n");
		return;
	}

	for (i = 0; i < PLIM; i++) {
		if (i == SIZE || buf[i] == '\0') {
			printf("\n");
			return;
		}
		printf("%c", buf[i]);
	}
	if (buf[i] == '\0') {
		printf("\n");
	} else {
		printf("...\n");
	}
}

	static void
hist_test(void)
{
#ifdef TKEY_FIXED
	tkey_fixed_t *basic_key[KEYS];
	tkey_fixed_hist_t *basic_hist;
	size_t basic_dist[3], basic_disc_index;
	uint64_t basic_tacc = 0;
	bool basic_disc_result;
#endif
#ifdef TKEY_DENSE
	tkey_dense_t *dense_key[KEYS];
	tkey_dense_hist_t *dense_hist;
	size_t dense_disc_index, dense_dist[3];
	uint64_t dense_tacc = 0;
	bool dense_disc_result; 
#endif
#ifdef TKEY_SPARSE
	tkey_sparse_t *sparse_key[KEYS];
	tkey_sparse_hist_t *sparse_hist;
	size_t sparse_disc_index, sparse_dist[3];
	uint64_t sparse_tacc = 0;
	bool sparse_disc_result;
#endif
#ifdef TKEY_HYBRID
	tkey_hybrid_t *hybrid_key[KEYS];
	tkey_hybrid_hist_t *hybrid_hist;
	size_t hybrid_disc_index, hybrid_dist[3];
	uint64_t hybrid_tacc = 0;
	bool	hybrid_disc_result; 
#endif
#ifdef TKEY_ULTRA
	tkey_ultra_t *ultra_key[KEYS];
	tkey_ultra_hist_t *ultra_hist;
	size_t ultra_disc_index, ultra_dist[3];
	uint64_t ultra_tacc = 0;
	bool	ultra_disc_result; 
#endif
#ifdef TKEY
	tkey_t *key[KEYS];
	tkey_hist_t *hist;
	size_t disc_index, dist[3];
	uint64_t tacc = 0;
	bool	disc_result; 
#endif
	ttype_t *type;
	size_t key_index1, key_index2, index, rf[5];

	uint64_t t0, t1, tn;
	char buf[666], pre[] = "[    ]", m2_buf[1 << 16];
	bool equal, subset, superset, intersect, disjoint, dflt_flag;

#ifdef TKEY_FIXED
	basic_key[0] = NULL;
	type = tkey_fixed_type(basic_key[0]);
#else
	tkey_type_create(&type, BASE, SIZE, DFLT, "hist_test");
#endif
	fprintf(stderr, "type: name = \"%s\", size = %zu, base = %s, default = %c\n\n",
			ttype_name(type), ttype_size(type), ttype_base(type) == TBASE_BINARY ? "BINARY" : "TERNARY", tbit_to_char(ttype_default(type)));
	printf("%-10s %s\n", "key index", "key bits"); 

	for (key_index1 = 0; key_index1 < KEYS; key_index1++) {
#ifdef TKEY_FIXED
		tkey_fixed_create(&basic_key[key_index1], type);
#endif
#ifdef TKEY_DENSE
		tkey_dense_create(&dense_key[key_index1], type);
#endif
#ifdef TKEY_SPARSE
		tkey_sparse_create(&sparse_key[key_index1], type);
#endif
#ifdef TKEY_HYBRID
		tkey_hybrid_create(&hybrid_key[key_index1], type);
#endif
#ifdef TKEY_ULTRA
		tkey_ultra_create(&ultra_key[key_index1], type);
#endif
#ifdef TKEY
		tkey_create(&key[key_index1], type);
#endif
#if 0
		tkey_put(key[key_index], key_index, TBIT1);
#else
		//		printf("R %10zu ", key_index);
		for (index = 0; index < SIZE; index++) {
#if 1
			uint64_t dice;
			tbit_t bit;

			if ((index % 8) == 0) {
				dice = lrand48() % KEYS;
				if (dice < key_index1) {
					dflt_flag = false;
				} else {
					dflt_flag = true;
				}
			}

			if (dflt_flag) {
				bit = TBIT_;
			} else {
				dice = lrand48() % 2;

				if (dice == 0) {
					bit = TBIT0;
				} else if (dice == 1) {
					bit = TBIT1;
				} else {
					bit = TBIT_;
				}
			}
#else
			uint64_t dice = lrand48() % 100;
			tbit_t bit;

			if (dice < (uint64_t)(25.0 * (0.5 * (float)key_index1) / (float)KEYS)) {
				bit = TBIT0;
				//				printf("0");
			} else if (dice < (uint64_t)(50.0 * (0.5 * (float)key_index1) / (float)KEYS)) {
				bit = TBIT1;
				//				printf("1");
			} else {
				if ((lrand48() % 1) == 0) {
					bit = TBIT_;
				} else {
					bit = DFLT;
				}
				//				printf("*");
			}
#endif
			ref_bit[index] = bit;
#ifdef TKEY_FIXED
			tkey_fixed_put(basic_key[key_index1], index, bit);
#endif
#ifdef TKEY_DENSE
			tkey_dense_put(dense_key[key_index1], index, bit);
#endif
#ifdef TKEY_SPARSE
			tkey_sparse_put(sparse_key[key_index1], index, bit);
#endif
#ifdef TKEY_HYBRID
			tkey_hybrid_put(hybrid_key[key_index1], index, bit);
#endif
#ifdef TKEY_ULTRA
			tkey_ultra_put(ultra_key[key_index1], index, bit);
#endif
#ifdef TKEY
			tkey_put(key[key_index1], index, bit);
			//			if (bit != tkey_get(key[key_index1], bit)) {
			//				fprintf(stderr, "block_index_put error index = %zu\n", index);
			//			}
#endif
		}
		//		printf("\n");
#endif
#ifdef TKEY_FIXED
#	ifdef VERBOSE
		//tkey_basic_preamble(basic_key[key_index1], pre);
		tkey_fixed_to_string(basic_key[key_index1], buf, 666);
		printf("B %10zu %s", key_index1, pre); keyprint(buf);
		for (index = 0; index < SIZE; index++) {
			assert(ref_bit[index] == tkey_fixed_get(basic_key[key_index1], index));
		}
#	endif
#endif
#ifdef TKEY_DENSE
#	ifdef VERBOSE
		tkey_dense_preamble(dense_key[key_index1], pre);
		tkey_dense_to_string(dense_key[key_index1], buf, 666);
		printf("D %10zu %s", key_index1, pre); keyprint(buf);
		for (index = 0; index < SIZE; index++) {
			assert(tkey_fixed_get(basic_key[key_index1], index) == tkey_dense_get(dense_key[key_index1], index));
		}
#	endif
#endif
#ifdef TKEY_SPARSE
#	ifdef VERBOSE
		tkey_sparse_preamble(sparse_key[key_index1], pre);
		tkey_sparse_to_string(sparse_key[key_index1], buf, 666);
		printf("S %10zu %s", key_index1, pre); keyprint(buf);
		for (index = 0; index < SIZE; index++) {
			assert(tkey_fixed_get(basic_key[key_index1], index) == tkey_sparse_get(sparse_key[key_index1], index));
		}
#	endif
#endif
#ifdef TKEY_HYBRID
#	ifdef VERBOSE
		tkey_hybrid_preamble(hybrid_key[key_index1], pre);
		tkey_hybrid_to_string(hybrid_key[key_index1], buf, 666);
		printf("H %10zu %s", key_index1, pre); keyprint(buf);
		for (index = 0; index < SIZE; index++) {
			assert(tkey_fixed_get(basic_key[key_index1], index) == tkey_hybrid_get(hybrid_key[key_index1], index));
		}
#	endif
#endif
#ifdef TKEY_ULTRA
#	ifdef VERBOSE
		tkey_ultra_preamble(ultra_key[key_index1], pre);
		tkey_ultra_to_string(ultra_key[key_index1], buf, 666);
		printf("U %10zu %s", key_index1, pre); keyprint(buf);
		for (index = 0; index < SIZE; index++) {
			assert(tkey_fixed_get(basic_key[key_index1], index) == tkey_ultra_get(ultra_key[key_index1], index));
		}
#	endif
#endif
#ifdef TKEY
#	ifdef VERBOSE
		//tkey_preamble(key[key_index1], pre);
		tkey_to_string(key[key_index1], buf, 666);
		printf("X %10zu %s", key_index1, pre); keyprint(buf);
		for (index = 0; index < SIZE; index++) {
			assert(tkey_fixed_get(basic_key[key_index1], index) == tkey_get(key[key_index1], index));
		}
#	endif
#endif
#ifdef VERBOSE
		printf("\n");
#else
		fprintf(stderr, ".");
#endif
		//getchar();
		//	printf("%10zu %s\n", key_index, buf);
	}

	printf("\n\n");

#ifdef HIST_REF
	printf("%-10s    %10s %10s %10s    %10s %10s %10s\n", "bit index", "tkey #0", "tkey #1", "tkey #*", "ref #0", "ref #1", "ref #*");

	for (index = 0; index < SIZE; index++) {
		hist0[index] = 0;
		hist1[index] = 0;
		hist_[index] = 0;
	}
#else
	printf("%-10s    %10s %10s %10s\n", "bit index", "tkey #0", "tkey #1", "tkey #*");
#endif

#ifdef TKEY_FIXED
	tkey_fixed_hist_create(&basic_hist, type);
#endif
#ifdef TKEY_DENSE
	tkey_dense_hist_create(&dense_hist, type);
#endif
#ifdef TKEY_SPARSE
	tkey_sparse_hist_create(&sparse_hist, type);
#endif
#ifdef TKEY_HYBRID
	tkey_hybrid_hist_create(&hybrid_hist, type);
#endif
#ifdef TKEY_ULTRA
	tkey_ultra_hist_create(&ultra_hist, type);
#endif
#ifdef TKEY	
	tkey_hist_create(&hist, type);
#endif

	fprintf(stderr, "histogram add: ");
	for (key_index1 = 0; key_index1 < KEYS; key_index1++) {
		fprintf(stderr, ".");
#ifdef TKEY_FIXED
		tkey_fixed_hist_add(basic_hist, basic_key[key_index1]);
#endif
#ifdef TKEY_DENSE
		tkey_dense_hist_add(dense_hist, dense_key[key_index1]);
#endif
#ifdef TKEY_SPARSE
		tkey_sparse_hist_add(sparse_hist, sparse_key[key_index1]);
#endif
#ifdef TKEY_HYBRID
		tkey_hybrid_hist_add(hybrid_hist, hybrid_key[key_index1]);
#endif
#ifdef TKEY_ULTRA
		tkey_ultra_hist_add(ultra_hist, ultra_key[key_index1]);
#endif
#ifdef TKEY
		tkey_hist_add(hist, key[key_index1]);
#endif
#ifdef HIST_REF
		for (index = 0; index < SIZE; index++) {
			switch (tkey_fixed_get(basic_key[key_index1], index)) {
				case TBIT0:
#ifdef TKEY_DENSE
					assert(tkey_dense_get(dense_key[key_index1], index) == TBIT0);
#endif
#ifdef TKEY_SPARSE
					assert(tkey_sparse_get(sparse_key[key_index1], index) == TBIT0);
#endif
#ifdef TKEY_HYBRID
					assert(tkey_hybrid_get(hybrid_key[key_index1], index) == TBIT0);
#endif
#ifdef TKEY_ULTRA
					assert(tkey_ultra_get(ultra_key[key_index1], index) == TBIT0);
#endif
#ifdef TKEY
					assert(tkey_get(key[key_index1], index) == TBIT0);
#endif
					++hist0[index];
					break;
				case TBIT1:
#ifdef TKEY_DENSE
					assert(tkey_dense_get(dense_key[key_index1], index) == TBIT1);
#endif
#ifdef TKEY_SPARSE
					assert(tkey_sparse_get(sparse_key[key_index1], index) == TBIT1);
#endif
#ifdef TKEY_HYBRID
					assert(tkey_hybrid_get(hybrid_key[key_index1], index) == TBIT1);
#endif
#ifdef TKEY_ULTRA
					assert(tkey_ultra_get(ultra_key[key_index1], index) == TBIT1);
#endif
#ifdef TKEY
					assert(tkey_get(key[key_index1], index) == TBIT1);
#endif
					++hist1[index];
					break;
				case TBIT_:
#ifdef TKEY_DENSE
					assert(tkey_dense_get(dense_key[key_index1], index) == TBIT_);
#endif
#ifdef TKEY_SPARSE
					assert(tkey_sparse_get(sparse_key[key_index1], index) == TBIT_);
#endif
#ifdef TKEY_HYBRID
					assert(tkey_hybrid_get(hybrid_key[key_index1], index) == TBIT_);
#endif
#ifdef TKEY_ULTRA
					assert(tkey_ultra_get(ultra_key[key_index1], index) == TBIT_);
#endif
#ifdef TKEY
					assert(tkey_get(key[key_index1], index) == TBIT_);
#endif
					++hist_[index];
					break;
				default:
					assert(0);
			}
		}
#endif
	}
	fprintf(stderr, "\n");

	for (index = 0; index < SIZE; index++) {
#ifdef TKEY_FIXED
		tkey_fixed_hist_dist(basic_hist, index, basic_dist);
#endif
#ifdef TKEY_DENSE
		tkey_dense_hist_dist(dense_hist, index, dense_dist);
#endif
#ifdef TKEY_SPARSE
		tkey_sparse_hist_dist(sparse_hist, index, sparse_dist);
#endif
#ifdef TKEY_HYBRID
		tkey_hybrid_hist_dist(hybrid_hist, index, hybrid_dist);
#endif
#ifdef TKEY_ULTRA
		tkey_ultra_hist_dist(ultra_hist, index, ultra_dist);
#endif
#ifdef TKEY
		tkey_hist_dist(hist, index, dist);
#endif
		printf("%10zu    %10zu %10zu %10zu    %10zu %10zu %10zu    ", index, basic_dist[0], basic_dist[1], basic_dist[2], hist0[index], hist1[index], hist_[index]);

		if ((basic_dist[0] == hist0[index]) && (basic_dist[1] == hist1[index]) && (basic_dist[2] == hist_[index])) {
			printf("OK\n");
		} else {
			printf("ERROR");
			getchar();
		}
#ifdef TKEY_DENSE
		assert(basic_dist[0] == dense_dist[0]);
		assert(basic_dist[1] == dense_dist[1]);
		assert(basic_dist[2] == dense_dist[2]);
#endif
#ifdef TKEY_SPARSE
		assert(basic_dist[0] == sparse_dist[0]);
		assert(basic_dist[1] == sparse_dist[1]);
		assert(basic_dist[2] == sparse_dist[2]);
#endif
#ifdef TKEY_HYBRID
		assert(basic_dist[0] == hybrid_dist[0]);
		assert(basic_dist[1] == hybrid_dist[1]);
		assert(basic_dist[2] == hybrid_dist[2]);
#endif
#ifdef TKEY_ULTRA
		assert(basic_dist[0] == ultra_dist[0]);
		assert(basic_dist[1] == ultra_dist[1]);
		assert(basic_dist[2] == ultra_dist[2]);
#endif
#ifdef TKEY
		assert(basic_dist[0] == dist[0]);
		assert(basic_dist[1] == dist[1]);
		assert(basic_dist[2] == dist[2]);
#endif
#ifdef HIST_REF
#else
		printf("%10zu    %10zu %10zu %10zu\n", index, basic_dist[0], basic_dist[1], basic_dist[2]);
#endif
	}

#ifdef TKEY_FIXED
	basic_disc_result = tkey_fixed_hist_disc(basic_hist, &basic_disc_index);
#endif
#ifdef TKEY_DENSE
	dense_disc_result = tkey_dense_hist_disc(dense_hist, &dense_disc_index);
#endif
#ifdef TKEY_SPARSE
	sparse_disc_result = tkey_sparse_hist_disc(sparse_hist, &sparse_disc_index);
#endif
#ifdef TKEY_HYBRID
	hybrid_disc_result = tkey_hybrid_hist_disc(hybrid_hist, &hybrid_disc_index);
#endif
#ifdef TKEY_ULTRA
	ultra_disc_result = tkey_ultra_hist_disc(ultra_hist, &ultra_disc_index);
#endif
#ifdef TKEY
	disc_result = tkey_hist_disc(hist, &disc_index);
#endif
	if (basic_disc_result) {
		printf("\n\n");
		printf("B discriminating bit index: %zu\n", basic_disc_index);
#ifdef TKEY_DENSE
		assert(dense_disc_result);
		printf("D discriminating bit index: %zu\n", dense_disc_index);
#endif
#ifdef TKEY_SPARSE
		assert(sparse_disc_result);
		printf("S discriminating bit index: %zu\n", sparse_disc_index);
#endif
#ifdef TKEY_HYBRID
		assert(hybrid_disc_result);
		printf("H discriminating bit index: %zu\n", hybrid_disc_index);
#endif
#ifdef TKEY_ULTRA
		assert(ultra_disc_result);
		printf("U discriminating bit index: %zu\n", ultra_disc_index);
#endif
#ifdef TKEY
		assert(disc_result);
		printf("X discriminating bit index: %zu\n", disc_index);
#endif
#ifdef TKEY_DENSE
		assert(dense_disc_index == basic_disc_index);
#endif
#ifdef TKEY_SPARSE
		assert(sparse_disc_index == basic_disc_index);
#endif
#ifdef TKEY_HYBRID
		assert(hybrid_disc_index == basic_disc_index);
#endif
#ifdef TKEY_ULTRA
		assert(ultra_disc_index == basic_disc_index);
#endif
#ifdef TKEY
		assert(disc_index == basic_disc_index);
#endif
	} else {
#ifdef TKEY_DENSE
		assert(!dense_disc_result);
#endif
#ifdef TKEY_SPARSE
		assert(!sparse_disc_result);
#endif
#ifdef TKEY_HYBRID
		assert(!hybrid_disc_result);
#endif
#ifdef TKEY_ULTRA
		assert(!ultra_disc_result);
#endif
#ifdef TKEY
		assert(!disc_result);
#endif
		printf("\n\ndiscrimination failed\n");
	}

	fprintf(stderr, "histogram del: ");
	for (key_index1 = 0; key_index1 < KEYS; key_index1++) {
		fprintf(stderr, ".");
		tkey_fixed_hist_del(basic_hist, basic_key[key_index1]);
#ifdef TKEY_DENSE
		tkey_dense_hist_del(dense_hist, dense_key[key_index1]);
#endif
#ifdef TKEY_SPARSE
		tkey_sparse_hist_del(sparse_hist, sparse_key[key_index1]);
#endif
#ifdef TKEY_HYBRID
		tkey_hybrid_hist_del(hybrid_hist, hybrid_key[key_index1]);
#endif
#ifdef TKEY_ULTRA
		tkey_ultra_hist_del(ultra_hist, ultra_key[key_index1]);
#endif
#ifdef TKEY
		tkey_hist_del(hist, key[key_index1]);
#endif
#ifdef HIST_REF
		for (index = 0; index < SIZE; index++) {
			switch (tkey_fixed_get(basic_key[key_index1], index)) {
				case TBIT0:
					assert(hist0[index] >= 0);
					--hist0[index];
					break;
				case TBIT1:
					assert(hist1[index] >= 0);
					--hist1[index];
					break;
				case TBIT_:
					assert(hist_[index] >= 0);
					--hist_[index];
					break;
				default:
					assert(0);
			}
		}
#endif
	}
	fprintf(stderr, "\n");
#ifdef HIST_REF
	//    fprintf(stderr, "\nEmpty hist check: ");
	for (index = 0; index < SIZE; index++) {
		tkey_fixed_hist_dist(basic_hist, index, basic_dist);
		assert(basic_dist[0] == 0);
		assert(basic_dist[1] == 0);
		assert(basic_dist[2] == 0);
#ifdef TKEY_DENSE
		tkey_dense_hist_dist(dense_hist, index, dense_dist);
		assert(dense_dist[0] == 0);
		assert(dense_dist[1] == 0);
		assert(dense_dist[2] == 0);
#endif
#ifdef TKEY_SPARSE
		tkey_sparse_hist_dist(sparse_hist, index, sparse_dist);
		assert(sparse_dist[0] == 0);
		assert(sparse_dist[1] == 0);
		assert(sparse_dist[2] == 0);
#endif
#ifdef TKEY_HYBRID
		tkey_hybrid_hist_dist(hybrid_hist, index, hybrid_dist);
		assert(hybrid_dist[0] == 0);
		assert(hybrid_dist[1] == 0);
		assert(hybrid_dist[2] == 0);
#endif
#ifdef TKEY_ULTRA
		tkey_ultra_hist_dist(ultra_hist, index, ultra_dist);
		assert(ultra_dist[0] == 0);
		assert(ultra_dist[1] == 0);
		assert(ultra_dist[2] == 0);
#endif
#ifdef TKEY
		tkey_hist_dist(hist, index, dist);
		assert(dist[0] == 0);
		assert(dist[1] == 0);
		assert(dist[2] == 0);
#endif
	}
#endif

	equal = subset = superset = intersect = disjoint = false;
	rf[0] = rf[1] = rf[2] = rf[3] = rf[4] = 0;
	tn = 0;

	fprintf(stderr, "relation: ");
	for (key_index1 = 0; key_index1 < KEYS - 1; key_index1++) {
		fprintf(stderr, ".");
		for (key_index2 = key_index1 + 1; key_index2 < KEYS; key_index2++) {
			relation_t basic_relation;
#ifdef TKEY_DENSE
			relation_t dense_relation;
#endif
#ifdef TKEY_SPARSE
			relation_t sparse_relation;
#endif
#ifdef TKEY_HYBRID
			relation_t hybrid_relation;
#endif
#ifdef TKEY_ULTRA
			relation_t ultra_relation;
#endif
#ifdef TKEY
			relation_t block_relation;
#endif
			t0 = bench_nanoseconds();
			basic_relation = tkey_fixed_relation(basic_key[key_index1], basic_key[key_index2]);
			t1 = bench_nanoseconds();
			basic_tacc += t1 - t0;
			++tn;
#ifdef TKEY_DENSE
			t0 = bench_nanoseconds();
			dense_relation = tkey_dense_relation(dense_key[key_index1], dense_key[key_index2]);
			t1 = bench_nanoseconds();
			dense_tacc += t1 - t0;
			assert(dense_relation == basic_relation);
#endif
#ifdef TKEY_SPARSE
			t0 = bench_nanoseconds();
			sparse_relation = tkey_sparse_relation(sparse_key[key_index1], sparse_key[key_index2]);
			t1 = bench_nanoseconds();
			sparse_tacc += t1 - t0;
			assert(sparse_relation == basic_relation);
#endif
#ifdef TKEY_HYBRID
			t0 = bench_nanoseconds();
			hybrid_relation = tkey_hybrid_relation(hybrid_key[key_index1], hybrid_key[key_index2]);
			t1 = bench_nanoseconds();
			hybrid_tacc += t1 - t0;
			assert(hybrid_relation == basic_relation);
#endif
#ifdef TKEY_ULTRA
			t0 = bench_nanoseconds();
			ultra_relation = tkey_ultra_relation(ultra_key[key_index1], ultra_key[key_index2]);
			t1 = bench_nanoseconds();
			ultra_tacc += t1 - t0;
			assert(ultra_relation == basic_relation);
#endif
#ifdef TKEY
			t0 = bench_nanoseconds();
			block_relation = tkey_relation(key[key_index1], key[key_index2]);
			t1 = bench_nanoseconds();
			tacc += t1 - t0;
			assert(block_relation == basic_relation);
#endif
			switch (basic_relation) {
				case RELATION_EQUAL:
					//		    printf("equal\n");
					if (!equal) {
						equal = true;
						//getchar();
						//			printf("\n");
					} else {
						//			printf("\n");
					}
					break;
				case RELATION_SUBSET:
					//		    printf("subset\n");
					if (!subset) {
						subset = true;
						//getchar();
						//			printf("\n");
					} else {
						//			printf("\n");
					}
					break;
				case RELATION_SUPERSET:
					assert(tkey_fixed_relation(basic_key[key_index2], basic_key[key_index1]) == RELATION_SUBSET);;
					//		    printf("superset\n");
					if (!superset) {
						superset = true;
						//getchar();
						//			printf("\n");
					} else {
						//			printf("\n");
					}
					break;
				case RELATION_INTERSECT:
					//		    printf("intersect\n");
					if (!intersect) {
						intersect = true;
						//getchar();
						//			printf("\n");
					} else {
						//			printf("\n");
					}
					break;
				case RELATION_DISJOINT:
					//		    printf("disjoint\n");
					if (!disjoint) {
						disjoint = true;
						//getchar();
						//			printf("\n");
					} else {
						//			printf("\n");
					}
					break;
				default:
					/* *** NOT REACHED *** */
					assert(0);
			}
			rf[basic_relation]++;
		}
	}
	fprintf(stderr, "\n");

	printf("B average time: %10lu nanoseconds\n", basic_tacc / tn);
#ifdef TKEY_DENSE
	printf("D average time: %10lu nanoseconds\n", dense_tacc / tn);
#endif
#ifdef TKEY_SPARSE
	printf("S average time: %10lu nanoseconds\n", sparse_tacc / tn);
#endif
#ifdef TKEY_HYBRID
	printf("H average time: %10lu nanoseconds\n", hybrid_tacc / tn);
#endif
#ifdef TKEY_ULTRA
	printf("U average time: %10lu nanoseconds\n", ultra_tacc / tn);
#endif
#ifdef TKEY
	printf("X average time: %10lu nanoseconds\n", tacc / tn);
#endif
	printf("\nRelation Frequencies\n");
	printf("%-16s %10zu\n", "equal", rf[0]);
	printf("%-16s %10zu\n", "subset", rf[1]);
	printf("%-16s %10zu\n", "superset", rf[2]);
	printf("%-16s %10zu\n", "intersect", rf[3]);
	printf("%-16s %10zu\n", "disjoint", rf[4]);

	m2_report(m2_buf, 1 << 16);
	fprintf(stderr, "\n\n%s\n", m2_buf);

	tkey_fixed_hist_destroy(&basic_hist);
#ifdef TKEY_DENSE
	tkey_dense_hist_destroy(&dense_hist);
#endif
#ifdef TKEY_SPARSE
	tkey_sparse_hist_destroy(&sparse_hist);
#endif
#ifdef TKEY_HYBRID
	tkey_hybrid_hist_destroy(&hybrid_hist);
#endif
#ifdef TKEY_ULTRA
	tkey_ultra_hist_destroy(&ultra_hist);
#endif
#ifdef TKEY
	tkey_hist_destroy(&hist);
#endif
	for (key_index1 = 0; key_index1 < KEYS; key_index1++) {
		tkey_fixed_destroy(&basic_key[key_index1]);
#ifdef TKEY_DENSE
		tkey_dense_destroy(&dense_key[key_index1]);
#endif
#ifdef TKEY_SPARSE
		tkey_sparse_destroy(&sparse_key[key_index1]);
#endif
#ifdef TKEY_HYBRID
		tkey_hybrid_destroy(&hybrid_key[key_index1]);
#endif
#ifdef TKEY_ULTRA
		tkey_ultra_destroy(&ultra_key[key_index1]);
#endif
#ifdef TKEY
		tkey_destroy(&key[key_index1]);
#endif
	}

	ttype_destroy(&type);

	m2_report(m2_buf, 1 << 16);
	fprintf(stderr, "\n\n%s\n", m2_buf);

	m2_exit();

	exit(0);
}

	int
main(int argc, char *argv[])
{
	//	tkey_type_t *type = NULL;
	//	tkey_dense_t *key1 = NULL;
	//	char buf[257];
	//	unsigned i;
	//	uint32_t index, hist[3];
	//	char implementation;

	//tkey_selftest();

	hist_test();
#if 0
	//#if TKEY_IMPLEMENTATION == TKEY_FIXED_IMPLEMENTATION
	//	type = tkey_basic_type;
	//#elif TKEY_IMPLEMENTATION == TKEY_SPARSE_IMPLEMENTATION
	//	tkey_type_create(&type, TKEY_BASE_TERNARY, W, TBIT_, "Type 1"); 
	//#endif
	//	tkey_create(&key1, type);
#if 0	
	fprintf(stderr, "\n");
	for (i = 0; i < W; i++) fprintf(stderr, "%d", i % 10);
	fprintf(stderr, "\n");
	tkey_to_string(key1, buf, 256);
	fprintf(stdout, "%s\n", buf);

	tkey_put_bit(key1, 0, TBIT1);

	fprintf(stderr, "\n");
	for (i = 0; i < W; i++) fprintf(stderr, "%d", i % 10);
	tkey_to_string(key1, buf, 256);
	fprintf(stderr, "\n");
	fprintf(stdout, "%s\n", buf);

	tkey_put_bit(key1, 47, TBIT1);

	fprintf(stderr, "\n");
	for (i = 0; i < W; i++) fprintf(stderr, "%d", i % 10);
	tkey_to_string(key1, buf, 256);
	fprintf(stderr, "\n");
	fprintf(stdout, "%s\n", buf);

	for (i = 0; i < W; i++) {
		switch (tkey_get_bit(key1, i)) {
			case TBIT0:
				fprintf(stderr, "0");
				break;
			case TBIT1:
				fprintf(stderr, "1");
				break;
			case TBIT_:
				fprintf(stderr, "*");
				break;
			default:
				/* *** NOT REACHED *** */
				assert(0);
		}
	}
	fprintf(stderr, "\n");
#endif

	hist[0] = hist[1] = 0;
	hist[2] = W;

	for (index = 0; index < W; index++) {
		simple_bit[index] = TBIT_;
	}
#if TKEY_IMPLEMENTATION == TKEY_FIXED_IMPLEMENTATION
	implementation = 'B';
#elif TKEY_IMPLEMENTATION == TKEY_SPARSE_IMPLEMENTATION
	implementation = 'S';
#endif

	for (uint32_t n = 1;; n++) {
		tbit_t bit;
		i = lrand48() % W;

		bit = tkey_get(key1, i);
		fprintf(stderr, "%c %10u  ", implementation, n);
retry:
		switch (lrand48() % 3) {
			case 0:
				if (bit == TBIT0) goto retry;
				fprintf(stderr, "key[%010u] <= 0", i);
				tkey_put(key1, i, TBIT0);
				simple_bit[i] = TBIT0;
				++hist[0];
				break;
			case 1:
				if (bit == TBIT1) goto retry;
				fprintf(stderr, "key[%010u] <= 1", i);
				tkey_put(key1, i, TBIT1);
				simple_bit[i] = TBIT1;
				++hist[1];
				break;
			case 2:
				if (bit == TBIT_) goto retry;
				fprintf(stderr, "key[%010u] <= *", i);
				tkey_put(key1, i, TBIT_);
				simple_bit[i] = TBIT_;
				++hist[2];
				break;
		}

		if (true || (n & 0xff) == 0) {
			for (index = 0; index < W; index++) {
				assert(tkey_get(key1, index) == simple_bit[index]);
			}
		}

		fprintf(stderr, " %10u %10u %10u  ", hist[0], hist[1], hist[2]);
		if (W <= 192) {
			tkey_to_string(key1, buf, 257);
			fprintf(stderr, "%s", buf);
		}
		fprintf(stderr, "\n");
	}
#endif
}
