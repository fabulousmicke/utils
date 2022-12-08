/**
 * @file ttype.c
 * @brief Ternary key type implementation.
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
#include <string.h>
#include <m2.h>

#include <ttype.h>

/** @brief CFDB type abstract data type implementation. */
struct ttype {
	size_t size;							/**< Key size. */
	tbase_t base;						/**< Key base. */
	tbit_t dflt;							/**< Default bit. */
	char name[TTYPE_NAME_MAXLEN];		/**< Type name. */
};

static m2_t *ttype_handle = NULL;

	void
ttype_create(
		ttype_t **type,
		tbase_t base,
		size_t size,
		tbit_t dflt,
		char *name)
{
	assert(dflt != TBIT_ || base == TBASE_TERNARY);

	if (ttype_handle == NULL) {
		ttype_handle = m2_create("ttype_t", sizeof(ttype_t));
	}
	(*type) = m2_reuse(ttype_handle, 1, false);
	(*type)->base = base;
	(*type)->size = size;
	(*type)->dflt = dflt;
	strncpy((*type)->name, name, TTYPE_NAME_MAXLEN);
	(*type)->name[TTYPE_NAME_MAXLEN - 1] = '\0';
}

	void
ttype_destroy(ttype_t **type)
{
	m2_recycle(ttype_handle, (*type), 1);
	(*type) = NULL;
}

	size_t
ttype_size(ttype_t *type)
{
	return type->size;
}

	tbit_t
ttype_default(ttype_t *type)
{
	return type->dflt;
}

	char *
ttype_name(ttype_t *type)
{
	return type->name;
}

	tbase_t
ttype_base(ttype_t *type)
{
	return type->base;
}

