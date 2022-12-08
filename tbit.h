/**
 * @file tbit.h
 * @brief Charlie Foxtrot DB bit definition.
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
#ifndef TBIT
#define TBIT

#include <stdbool.h>
#include <assert.h>

/**
 * @brief CFDB ternary bit data type.
 */
typedef enum {
	TBIT0,	/**< Represents 0, cleared bit or boolean false. */ 
	TBIT1,	/**< Represents 1, set bit or boolean true. */
	TBIT_	/**< Represents *, wildcard bit or boolean "don't care". */
} tbit_t;

/**
 * @brief Convert ternary bit to character.
 *
 * @param bit Ternary bit to be converted.
 *
 * @return Character corresponding to bit.
 */
	static inline char
tbit_to_char(tbit_t bit)
{
	switch (bit) {
		case TBIT0: return '0';
		case TBIT1: return '1';
		case TBIT_: return '*';
		default:
					/* *** NOT REACHED *** */
					assert(0);
	}
}

/**
 * @brief Convert Character to ternary bit.
 *
 * @param c Character to convert.
 * @param bit Location of targer TBIT.
 *
 * @return Boolean true on success and false otherwise.
 *
 * @note On failure no write to bit location occurs.
 */
	static inline bool
tbit_from_char(
		char c,
		tbit_t *bit)
{
	switch (c) {
		case '0':
			(*bit) = TBIT0;
			break;
		case '1':
			(*bit) = TBIT1;
			break;
		case '*':
			(*bit) = TBIT_;
			break;
		default:
			/*
			 * Invalid character.
			 */
			return false;;
	}
	/*
	 * Success.
	 */
	return true;
}

#endif /* TBIT */
