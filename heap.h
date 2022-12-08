/**
 * @file heap.h
 * @brief Generic heap definition.
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
#ifndef HEAP
#define HEAP

#include <stdlib.h>

/** Heap abstract data type definition (hidden). */
struct heap;
typedef struct heap heap_t;

/**
 * @brief Create a new heap.
 *
 * Create a new heap of specified maximum size using caller provided comparison function.
 *
 * @param heap Address to varable that will contain created Heap.
 * @param maxsize Maximum number of elements that can be stored in Heap.
 * @param compare Comparison function to use when determining priority between elements.
 *
 * @note The comparison function must work as follows:
 * compare(x, y) = -1 => x has _higher_ priority than y.
 * compare(x, y) =  0 => x and y has the same priority.
 * compare(x, y) =  1 => x has _lower_ priority than y.
 *
 * See also definition of heap_extract_max below.
 */
void heap_create(
		heap_t **heap,
		size_t maxsize,
		int (*compare)(const void *, const void *));

/**
 * @brief Destroy a Heap.
 *
 * Destroy a heap and recycle all allocated memory (except for the elements).
 *
 * @param heap Address of Heap to be destroyed.
 */
void heap_destroy(heap_t **heap);

/**
 * @brief Heap size.
 *
 * Compute current number of elements in a Heap.
 *
 * @param heap Heap to determine size of.
 *
 * @return Number of elements in Heap.
 */
size_t heap_size(heap_t *heap);

/**
 * @brief Insert key in Heap.
 *
 * Copies and inserts a key, which can be anything that can be cast to a void * and back,
 * in the Heap.
 *
 * @param heap Heap to insert the new key in.
 * @param key Key to be inserted in Heap.
 */
void heap_insert(heap_t *heap, void *key);

/**
 * @brief Extract element with maximum priority.
 *
 * Extract element with maximum priority from Heap and return a pointer to it.
 *
 * @param heap Heap to extract element with maximum priority from.
 *
 * @return Pointer to the extracted key.
 */
void *heap_extract(heap_t *heap);

#endif
