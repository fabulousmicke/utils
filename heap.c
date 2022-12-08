/**
 * @file heap.c
 * @brief Generic heap implementation.
 * @author Mikael Sundstrom <micke@fabinv.com>
 *
 * @copyright Copyright (c) 2018 Fabulous Inventions AB - all rights reserved.
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
 * 
 */
#include <assert.h>
#include <m2.h>

#include <heap.h>

typedef void heap_void_t;

struct heap {
	void **base;	/* Base of heap array. */
	size_t maxsize;	/* Maximum number of elements. */
	size_t size;		/* Current number of elements. */

	/* 
	 * Comparison function (see qsort).
	 */
	int (*compare)(const void *, const void *);
};

static m2_t *heap_handle = NULL;

static m2_t *heap_voidptr_handle = NULL;

/*
 * Create a new heap.
 */
	void
heap_create(
		heap_t **heap,									/* the heap */
		size_t maxsize,									/* max size */
		int (*compare )(const void *, const void *))	/* cmp func */
{
	heap_t *result;

	//assert(heap != NULL);
	*heap = NULL;

	if (heap_handle == NULL) {
		heap_handle = m2_create("heap_t", sizeof(heap_t));
		if (heap_handle == NULL) return;
	}
	if (heap_voidptr_handle == NULL) {
		heap_voidptr_handle = m2_create("heap_void *", sizeof(void *));
		if (heap_voidptr_handle == NULL) return;
	}
	result = (heap_t *)m2_reuse(heap_handle, 1, false);
	if (result == NULL) return;

	result->base = (void **)m2_reuse(heap_voidptr_handle, maxsize, false);
	if (result->base == NULL) {
		m2_recycle(heap_handle, result, 1);
		return;
	}

	--result->base;	/* start with index 1 */

	result->compare = compare;

	result->maxsize = maxsize;

	result->size = 0;

	*heap = result;
}

	void
heap_destroy(heap_t **heap)
{
	m2_recycle(heap_voidptr_handle, (*heap)->base + 1, (*heap)->maxsize);
	m2_recycle(heap_handle, *heap, 1);
	*heap = NULL;
}


/*
 * Returns "current" number of elements in <heap>.
 */
	size_t
heap_size(heap_t *heap)
{
	return heap->size;
}

	static inline size_t
heap_parent(size_t i)
{
	return i >> 1;
}

	static inline size_t
heap_left(size_t i)
{
	return i << 1;
}

	static inline size_t
heap_right(size_t i)
{
	return (i << 1) + 1;
}

	static inline void
heap_swap(heap_t *heap, size_t i, size_t j)
{
	void *tmp;

	tmp = heap->base[i];
	heap->base[i] = heap->base[j];
	heap->base[j] = tmp;
}

	static inline void
heap_ify(heap_t *heap, size_t i)
{
	size_t left  = heap_left(i),
		   right = heap_right(i),
		   max;

	if ((left <= heap->size) &&
			(heap->compare(heap->base[left], heap->base[i]) == -1))
	{
		max = left;
	} else {
		max = i;
	}

	if ((right <= heap->size) &&
			(heap->compare(heap->base[right], heap->base[max]) == - 1))
	{
		max = right;
	}

	if (max != i) {
		heap_swap(heap, i, max);
		heap_ify(heap, max);
	}
}

/*
 * Copies and inserts <element> in <heap>.
 */
	void
heap_insert(heap_t *heap, void *key)
{
	size_t i;

	assert(heap->size < heap->maxsize);

	++heap->size;

	for (i = heap->size;
			(i > 1) && (heap->compare(heap->base[heap_parent(i)], key) == 1);
			i = heap_parent(i))
	{
		heap->base[i] = heap->base[heap_parent(i)];
	}

	heap->base[i] = key;
}

/*
 * Extract element with maximum priority from <heap> and returns a pointer.
 */
	void *
heap_extract(heap_t *heap)
{
	void *result;

	if (heap->size <= 0) return NULL;

	result = heap->base[1];

	heap->base[1] = heap->base[heap->size];

	--heap->size;

	heap_ify(heap, 1);

	return result;
}
