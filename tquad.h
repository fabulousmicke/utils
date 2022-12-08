/**
 * @file tquad.h
 * @brief Quad ternary byte (32-tbit) definition.
 * @author Mikael Sundstrom <micke@fabinv.com>
 *
 * Ternary quads are represented as 64-bit unsigned integers (uint64_t) where
 * the low 32 bits represent the value of the specified bits (zero by default
 * if the bit is not set) and the high 32 bits represents a mask indicating
 * whether the corresponding bit is specified (mas bit set) or wildcard (mask
 * bit cleared). I.e. representing the ternary word "...01*", where "*" denotes
 * wild card is achieved by setting the low 32 bits to "...01*" and the high 32
 * bits to "...110". The data type is deliberately not represented as an opaque
 * ADT but rather transparently as an uint64_t in order to be easily used as
 * bulding block in other data structures, easily passed as argument to- and
 * returned as result from function calls. Consequently, any uint64_t can be
 * used/passed as a tquad_t and vice versa, thus by-passing type checking.
 * Some degree of care is advised. 
 *
 * Each ternary quad represents a set of unsigned 32-bit integers that matches
 * the ternary quad, where a match between an unsigned 32-bit integer x[0..31]
 * and a ternary quad q[0..31] is defined as follows: For all i = 0..31, either
 * q[i] == TBIT_, x[i] == 0 and q[i] == TBIT0, or x[i] == 1 and q[i] == TBIT1.
 *
 * We do not distinguish between a ternary quad and the set of unsigned 32-bit
 * integers it reepresents when it is clear from the context. Note also that
 * not all sets of unsigned 32-bit integers can be represented by a single
 * ternary quad. For example the set {1, 2} requires two ternary quads
 * 00000000000000000000000000000001 and 00000000000000000000000000000010.
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
#ifndef TQUAD
#define TQUAD

#include <stdio.h>
#include <stdint.h>

#include <tbit.h>
#include <relation.h>

#define TQUAD_BITS ((size_t)32)

/**
 * @brief Transparent ternary quad data type.
 */
typedef uint64_t tquad_t;

/**
 * @brief Create ternary quad from 32-bit bits and mask.
 *
 * @param bits Specified bits (wildcard bits should be set to 0).
 * @param mask Mask indicating specified (1) and wildcard (0) bits.
 *
 * @return Created ternary quad.
 */ 
	static inline
tquad_t tquad(uint32_t bits, uint32_t mask)
{
	bits &= mask;
	return (tquad_t)bits | ((tquad_t)mask << 32);
}

/**
 * @brief Assign a value to ternary quad bit with a given index.
 *
 * @param quad Ternary quad containing the target bit.
 * @param bit Ternary bit value.
 * @param index Index of target bit.
 */
	static inline void
tquad_put(
		tquad_t *quad,
		size_t index,
		tbit_t bit)
{
	uint32_t bits, mask;

	assert(index < TQUAD_BITS);

	bits = (uint32_t)((uint64_t)(*quad) & 0xffffffff);
	mask = (uint32_t)((uint64_t)(*quad) >> 32);

	switch (bit) {
		case TBIT0:
			bits &= ~(0x00000001 << index);
			mask |= 0x00000001 << index;
			break;
		case TBIT1:
			bits |= 0x00000001 << index;
			mask |= 0x00000001 << index;
			break;
		case TBIT_:
			bits &= ~(0x00000001 << index);
			mask &= ~(0x00000001 << index);
			break;
		default:
			/* *** NOT REACHED *** */
			assert(0);
	}

	(*quad) = (tquad_t)bits | ((tquad_t)mask << 32);
}

/**
 * @brief Extract value of ternary quad bit with a given index.
 *
 * @param quad Ternary quad containing the target bit..
 * @param index Index of target bit.
 */
	static inline tbit_t
tquad_get(
		tquad_t quad,
		size_t index)
{
	uint32_t bits, mask;

	assert(index < TQUAD_BITS);

	bits = (uint32_t)(quad & 0xffffffff);
	mask = (uint32_t)(quad >> 32);

	if ((mask >> index) & 0x00000001) {
		return ((bits >> index) & 0x00000001) ? TBIT1 : TBIT0;
	}

	return TBIT_;
}

/**
 * @brief Convert Ternary quad to string and store in buffer.
 *
 * @param quad Ternary quad.
 * @param str Destination character buffer.
 * @param len Length of character buffer (must be at least TQUAD_BITS + 1).
 */
	static inline void
tquad_to_string(
		tquad_t quad,
		char *str,
		size_t len)
{
	size_t index;

	assert(str != NULL);
	assert(TQUAD_BITS < len);

	for (index = TQUAD_BITS - 1;; index--) {
		(*str++) = tbit_to_char(tquad_get(quad, index));

		if (index == 0) break;
	}
	str[index] = '\0';
}

/**
 * @brief Convert string to ternary quad.
 *
 * @param str Source character buffer.
 * @param quad Location of target ternary quad.
 *
 * @return Boolean true on success and false otherwise.
 *
 * @note On failure quad remains unaltered. 
 */
	static inline bool
tquad_from_string(
		char *str,
		tquad_t *quad)
{
	tquad_t result;
	size_t index;

	assert(str != NULL);

	result = (tquad_t)0x0000000000000000;

	for (index = TQUAD_BITS - 1; (*str) != '\0'; str++, index--) {
		tbit_t bit;

		if (!tbit_from_char((*str), &bit)) {
			/*
			 * Failure due to invalid character.
			 */
			return false;
		}

		tquad_put(&result, index, bit);

		if (index == 0) break;
	}

	if (index == 0) {
		/*
		 * Success.
		 */
		(*quad) = result;
		return true;
	}

	/*
	 * Failure due to unexpected end of string.
	 */
	return false;
}

/**
 * @brief Compute the set relation between the first- and second ternary quad.
 *
 * @param quad1 First ternary quad.
 * @param quad2 Second ternary quad.
 *
 * @return Set relation between first- and second ternary quad.
 */
	static inline relation_t
tquad_relation(
		tquad_t quad1,
		tquad_t quad2)
{
	uint32_t bits1, bits2, mask1, mask2;

	if (quad1 == quad2) {
		/*
		 * First and second quad are identical.
		 */
		return RELATION_EQUAL;
	}

	bits1 = (uint32_t)((uint64_t)quad1 & 0xffffffff);
	mask1 = (uint32_t)((uint64_t)quad1 >> 32);
	bits2 = (uint32_t)((uint64_t)quad2 & 0xffffffff);
	mask2 = (uint32_t)((uint64_t)quad2 >> 32);

	if (mask1 == mask2) {
		/*
		 * Specified bits are mismatching, quads are disjoint.
		 */
		assert(bits1 != bits2);
		return RELATION_DISJOINT;
	} else {
		/*
		 * Masks are mismatching.
		 */
		if (mask1 == (mask1 & mask2)) {
			/*
			 * First quad is less specified than second quad.
			 */
			if (bits1 == (bits2 & mask1)) {
				/*
				 * First quad is a superset of second quad.
				 */
				return RELATION_SUPERSET;
			}
			/*
			 * First and second quad are disjoint.
			 */
			return RELATION_DISJOINT;
		} else if ((mask1 & mask2) == mask2) {
			/*
			 * First quad is more specified than second quad.
			 */
			if (bits2 == (bits1 & mask2)) {
				/*
				 * First quad is a subset of second quad.
				 */
				return RELATION_SUBSET;
			}
			/*
			 * First and second quad are disjoint.
			 */
			return RELATION_DISJOINT;
		} else {
			uint32_t disc = mask1 & mask2;

			if ((bits1 & disc) == (bits2 & disc)) {
				/*
				 * First and second quad intersects.
				 */
				return RELATION_INTERSECT;
			}
			/*
			 * First and second quad are disjoint.
			 */
			return RELATION_DISJOINT;
		}
	}
}

/**
 * @brief Determine whether an (uint32_t) element is a member (i.e. matches) a ternary quad.
 *
 * @param quad Ternary quad.
 * @param element Element to be tested for membership.
 *
 * @return Boolean true if element is a member and false otherwise.
 */
	static inline bool
tquad_member(
		tquad_t quad,
		uint32_t element)
{
	uint32_t bits, mask;

	bits = (uint32_t)((uint64_t)quad & 0xffffffff);
	mask = (uint32_t)((uint64_t)quad >> 32);
	return (bool)((element & mask) == bits);
}

/**
 * @brief Compute set cardinality of ternary quad.
 *
 * @param quad Ternary quad.
 *
 * @return cardinality of ternary quad.
 */
	static inline size_t
tquad_cardinality(tquad_t quad)
{
	size_t index, result;

	result = 1;
	for (index = 0; index < TQUAD_BITS; index++) {
		if (tquad_get(quad, index) == TBIT_) {
			result <<= 1;
		}
	}

	return result;
}

/*
 * TODO
 *
 * Implement iterator that can be used to loop through all elements of a quad.
 */

#endif /* TQUAD */
