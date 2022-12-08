/**
 * @file ttype.h
 * @brief Ternary key type definition.
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
#ifndef TTYPE
#define TTYPE

#include <stdlib.h>
#include <tbase.h>
#include <tbit.h>

/** @brief Maximum ternary key/type size. */
#define TTYPE_SIZE_MAX ((size_t)0xffffffff)

/** @brief Maximum number of characters in ternary type name including ending NULL character. */
#define TTYPE_NAME_MAXLEN 32

/** @brief ternary type abstract data type definition. */
struct ttype;
typedef struct ttype ttype_t;

/**
 * @brief Create ternary type.
 *
 * @param type Location of type to be created.
 * @param base Key base - typically ternary but restriction to binary is possible.
 * @param size Number of bits (numbering/indexing always starts with zero).
 * @param dflt Default bit value which all bits are set to when a new key is created.
 * @param name Type name.
 */
void
ttype_create(
		ttype_t **type,
		tbase_t base,
		size_t size,
		tbit_t dflt,
		char *name);

/**
 * @brief Destroy ternary type.
 *
 * @param type Location of type to be destroyed.
 *
 * @note Make sure that all ternary instances and ternary keys referencing the type are destroyed first.
 */
void
ttype_destroy(ttype_t **type);

/**
 * @brief Return size of ternary type.
 *
 * @param type Ternary key type instance.
 *
 * @return Size in bits of ternary key type.
 */
size_t ttype_size(ttype_t *type);

/**
 * @brief Return default bit of ternary key type.
 *
 * @param type Ternary key type instance.
 *
 * @return Default bit of ternary key type.
 */
tbit_t ttype_default(ttype_t *type);

/**
 * @brief Return name of ternary type.
 *
 * @param type Ternary key type inistance.
 *
 * @return Name of ternary key type.
 */
char *ttype_name(ttype_t *type);

/**
 * @brief Return key base of ternary key type.
 *
 * @param type Ternary key type instance.
 *
 * @return Ternary key type base.
 */
tbase_t ttype_base(ttype_t *type);

#endif /* TTYPE */
