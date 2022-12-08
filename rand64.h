/**
 * @file rand64.h
 * @brief 64-bit Random number generator.
 * @author Mikael Sundström <micke@fabinv.com>
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
 *
 */
#ifndef RAND64
#define RAND64

#ifndef RAND64_NO_INIT
	static void
rand64_init(
		uint64_t seed)
{
	unsigned short seed16v[3];

	seed16v[0] = (unsigned short)((seed >> 0l) & 0xffffl);
	seed16v[1] = (unsigned short)((seed >> 16l) & 0xffffl);
	seed16v[2] = (unsigned short)((seed >> 32l) & 0xffffl);
	(void)seed48(seed16v);
}
#endif

	static inline uint64_t
rand64(void)
{
	uint64_t l, m, h;

	l = (((uint64_t)lrand48()) << 0) & 0x000000000000ffff;
	m = (((uint64_t)lrand48()) << 16) & 0x000000ffffff0000;
	h = (((uint64_t)lrand48()) << 40) & 0xffffff0000000000;

	return h | m | l;
}

#endif
