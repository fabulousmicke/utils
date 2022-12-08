/**
 * @file qkey.h
 * @brief Quantum key definition.
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
#ifndef QKEY
#define QKEY

#include <tkey.h>

#define qkey_t tkey_hist_t

#define QKEY_ZDEN_CAP 0.0
#define QKEY_ZNUM_CAP 0.0
#define QKEY_ZDEN_WGT 1.0
#define QKEY_ZNUM_WGT 1.0

	static inline float
qkey_distance(
		qkey_t *qkey,
		tkey_t *key,
		bool incl)
{
	ttype_t *type;
	size_t i, count, dist[3];
	float znum = 0.0, zden = 0.0, n, n0, n1;

	type = tkey_type(key);
	assert(type == tkey_hist_type(qkey));

	count = tkey_hist_count(qkey);

	if (count == 0 || (incl && count == 1)) return 0.0;

	assert(ttype_size(type) == TKEY_SIZE);

	n = (float)count;
	if (incl) n -= 1.0;

	for (i = 0; i < TKEY_SIZE; i++) {
		tkey_hist_dist(qkey, i, dist);
		n0 = (float)dist[TBIT0];
		n1 = (float)dist[TBIT1];
		switch (tkey_get(key, i)) {
			case TBIT0:
				zden += n1 / n;
				break;
			case TBIT1:
				zden += n0 / n;
				break;
			case TBIT_:
				znum += (n1 * n0) / (n * n);
				break;
			default:
				/* *** NOT REACHED *** */
				assert(0);
		}
	}

	if (zden < QKEY_ZDEN_CAP) {
		zden = 0.0;
	} else {
		zden -= QKEY_ZDEN_CAP;
	}
	zden *= QKEY_ZDEN_WGT;

	if (znum < QKEY_ZNUM_CAP) {
		znum = 0.0;
	} else {
		znum -= QKEY_ZNUM_CAP;
	}
	znum *= QKEY_ZNUM_WGT;

	if (zden == 0.0) return znum == 0.0 ? 1.0 : FLT_MAX;

	return znum / zden;
}

#endif /* QKEY */
