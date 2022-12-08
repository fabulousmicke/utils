/**
 * @file relation.h
 * @brief Set relationship type definition.
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
#ifndef RELATION
#define RELATION

#include <assert.h>

/** 
 * @brief Relationship between a pair of sets S1 and S2. 
 * 
 */ 
typedef enum { 
	RELATION_EQUAL,			/**< S1 is identical to S2. (implies inproper SUBSET, SUPERSET and INTERSECTING). */ 
	RELATION_SUBSET,		/**< S1 is a proper subset of S2 (but the sets are not equal). */ 
	RELATION_SUPERSET,		/**< S1 is a proper superset of S2 (but the sets are not equal). */ 
	RELATION_INTERSECT,		/**< S1 and S2 intersects but neither set is a subset/superset of the other set. */ 
	RELATION_DISJOINT,		/**< S1 and S2 are disjoint. */
} relation_t;

	static inline char *
relation_to_string(relation_t relation)
{
	switch (relation) {
		case RELATION_EQUAL:	return "equal";
		case RELATION_SUBSET:	return "subset";
		case RELATION_SUPERSET:	return "superset";
		case RELATION_INTERSECT:return "intersect";
		case RELATION_DISJOINT:	return "disjoint";
		default:
								/* *** NOT REACHED *** */
								assert(0);
	}
}

	static inline void
relation_update(
		relation_t *acc,
		relation_t cur)
{
	switch (cur) {
		case RELATION_EQUAL:
			/* Does not change anything. */
			break;
		case RELATION_SUBSET:
			if ((*acc) == RELATION_EQUAL) {
				(*acc) = RELATION_SUBSET;
			} else if ((*acc) == RELATION_SUPERSET) {
				(*acc) = RELATION_INTERSECT;
			}
			break;
		case RELATION_SUPERSET:
			if ((*acc) == RELATION_EQUAL) {
				(*acc) = RELATION_SUPERSET;
			} else if ((*acc) == RELATION_SUBSET) { 
				(*acc) = RELATION_INTERSECT;
			}
			break;
		case RELATION_INTERSECT:
			if ((*acc) != RELATION_DISJOINT) {
				(*acc) = RELATION_INTERSECT;
			}
			break;
		case RELATION_DISJOINT:
			(*acc) = RELATION_DISJOINT;
			break;
		default:
			/* *** NOT REACHED *** */
			assert(0);
	}
}

#endif /* RELATION */
