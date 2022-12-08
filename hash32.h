/**
 * @file hash32.h
 * @brief Hash utilities definition.
 * @author Mikael Sundstrom <micke@fabinv.com>
 *
 * @copyright Copyright (c) 2020 Fabulous Inventions AB - all rights reserved.
 * @copyright Copyright (c) 2020-2021 Mosys, Inc. - all rights reserved.
 *
 * This code is the confidential and proprietary property of Mosys, Inc. and
 * the possession or use of this file requires a written license from Mosys.
 *
 */
#ifndef HASH32
#define HASH32

#define HASH32_PERMUTE
//#define HASH32_VERBOSE

#define HASH32_SHIFT ((uint32_t)32u)
#define HASH32_PERM ((uint32_t)(1u * 2u * 3u * 4u * 5u * 6u * 7u * 8u))

	static inline uint32_t
hash32_full_avalanche(uint32_t a)
{
	/*****************************************************************************
	 * Hash functions from http://burtleburtle.net/bob/hash/integer.html         *
	 * Author: Bob Jenkins                                                       *
	 *****************************************************************************/
	a = (a + 0x7ed55d16) + (a << 12);
	a = (a ^ 0xc761c23c) ^ (a >> 19);
	a = (a + 0x165667b1) + (a << 5);
	a = (a + 0xd3a2646c) ^ (a << 9);
	a = (a + 0xfd7046c5) + (a << 3);
	a = (a ^ 0xb55a4f09) ^ (a >> 16);
	return a;
}

	static inline void
hash32_init(
		uint32_t *hash,
		uint32_t quads)
{
	for (size_t i = 0; i < quads; i++) {
		hash[i] = 0;
	}
}

	static inline void
hash32_hash(
		uint32_t *hash,
		uint32_t quads,
		uint32_t data,
		uint32_t index)
{
	uint32_t quad,
			 shift,
			 perm,
			 mask,
#ifdef HASH32_PERMUTE
			 fact,
			 order[] = { 0, 1, 2, 3, 4, 5, 6, 7 },
			 swap,
#endif
			 tmp;

#ifdef HASH32_VERBOSE
	fprintf(stderr, "=========================================================\n");
	fprintf(stderr, "data :");
	for (int i = 31; i >= 0; i--) {
		if ((i + 1) % 4 == 0) fprintf(stderr, " ");
		fprintf(stderr, "%u", (data >> (uint32_t)i) & 1);
	}
	fprintf(stderr, " : original\n\n");

	fprintf(stderr, "index: %10u : original\n\n", index);
#endif

	data = hash32_full_avalanche(data);
#ifdef HASH32_SCRAMBLE_INDEX
	index = hash32_full_avalanche(index);
#endif

#ifdef HASH32_VERBOSE
	fprintf(stderr, "data :");
	for (int i = 31; i >= 0; i--) {
		if ((i + 1) % 4 == 0) fprintf(stderr, " ");
		fprintf(stderr, "%u", (data >> (uint32_t)i) & 1);
	}
	fprintf(stderr, " : hashed\n\n");

	fprintf(stderr, "index: %10u : hashed\n\n", index);
#endif

#ifdef HASH32_MODULO_CHECK
	uint32_t q, s, p;
#endif
	quad = index;
	index /= quads;
#ifdef HASH32_MODULO_CHECK
	q = quad;
	q %= quads;
#endif
	quad -= index * quads;
#ifdef HASH32_MODULO_CHECK
	assert(q == quad);
#endif
	shift = index;
	index /= HASH32_SHIFT;
#ifdef HASH32_MODULO_CHECK
	s = shift;
	s %= HASH32_SHIFT;
#endif
	shift -= index * HASH32_SHIFT;
#ifdef HASH32_MODULO_CHECK
	assert(s == shift);
#endif
	perm = index;
	index /= HASH32_PERM;
#ifdef HASH32_MODULO_CHECK
	p = perm;
	p %= HASH32_PERM;
#endif
	perm -= index * HASH32_PERM;
#ifdef HASH32_MODULO_CHECK
	assert(p == perm);
#endif

#ifdef HASH32_VERBOSE
	fprintf(stderr, "quad : %10u\nshift: %10u\n\n", quad, shift);
#endif

	tmp = data;
	mask = (1u << shift) - 1u;
	data = (tmp & mask) << (32 - shift);
	data |= (tmp >> shift);

#ifdef HASH32_VERBOSE
	fprintf(stderr, "data :");
	for (int i = 31; i >= 0; i--) {
		if ((i + 1) % 4 == 0) fprintf(stderr, " ");
		fprintf(stderr, "%u", (data >> (uint32_t)i) & 1);
	}
	fprintf(stderr, " : shifted\n\n");
#endif
#ifdef HASH32_PERMUTE
	fact = HASH32_PERM;

	for (uint32_t src = 7; src; src--) {
		uint32_t dst;

#ifdef HASH32_VERBOSE
		fprintf(stderr, "src  : %10u\nperm : %10u\nfact : %10u\n", src, perm, fact);
#endif

		assert((0 <= perm) && (perm < fact));

		fact /= src + 1;
		swap = perm / fact;

		dst = src - swap;

#ifdef HASH32_VERBOSE
		fprintf(stderr, "swap : %10u\ndst  : %10u\n\n", swap, dst);
#endif

		assert(dst <= src);

		if (dst < src) {
			tmp = order[src];
			order[src] = order[dst];
			order[dst] = tmp;
		}

		perm -= swap * fact;
	}

	tmp = data;
	data = 0;
#ifdef HASH32_VERBOSE
	fprintf(stderr, "order: ");
#endif
	for (uint32_t pos = 0; pos <= 7; pos++) {
#ifdef HASH32_VERBOSE
		fprintf(stderr, "%10u", order[pos]);
#endif
		data |= ((tmp >> (order[pos] << 2) & 0xf) << (pos << 2));
	}
#ifdef HASH32_VERBOSE
	fprintf(stderr, "\n\ndata :");
	for (int i = 31; i >= 0; i--) {
		if ((i + 1) % 4 == 0) fprintf(stderr, " ");
		fprintf(stderr, "%u", (data >> (uint32_t)i) & 1);
	}
	fprintf(stderr, " : permuted\n");
#endif
#endif

	hash[quad] ^= data;
}

#endif /* HASH32 */
