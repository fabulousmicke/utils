/**
 * @file tquad_main.c
 * @brief Test program for 32-bit TCAM cells.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <tquad.h>

	static bool
tquad_equal(tquad_t x, tquad_t y) 
{
	relation_t r = tquad_relation(x, y);
	char str[33];

	if (r == RELATION_EQUAL) return true;
	tquad_to_string(x, str, 33);
	fprintf(stderr, "\n%s\n", str);
	tquad_to_string(y, str, 33);
	fprintf(stderr, "%s\n", str);
	return false;
}


	static void
selftest(size_t width)
{
	tquad_t x, y, tmp = 0;
	uint32_t bx, mx, by, my, e;
	relation_t r;
	char xs[33], ys[33], es[33];
	bool x_only, y_only, x_and_y, x_memb, y_memb;

	for (;;) {
		bx = lrand48() % (1 << width);
		mx = lrand48() % (1 << width);
		x = tquad(bx, mx);

		by = lrand48() % (1 << width);
		my = lrand48() % (1 << width);
		y = tquad(by, my);

		r = tquad_relation(x, y);

		fprintf(stderr, "|x| = %zu, |y| = %zu\n", tquad_cardinality(x), tquad_cardinality(y));

		printf("-----------------------------------------------------------------------------------------------\n");

		switch (r) {
			case RELATION_EQUAL:
				printf("%-12s: ", "equal");

				tquad_to_string(x, xs, 33);
				assert(tquad_from_string(xs, &tmp));
				assert(tquad_equal(x, tmp));

				tquad_to_string(y, ys, 33);
				assert(tquad_from_string(ys, &tmp));
				assert(tquad_equal(y, tmp));

				printf("%s\n%12s  %s\n", xs, "", ys);

				for (e = 0; e < (uint32_t)(1 << width); e++) {

					x_memb = tquad_member(x, e);
					y_memb = tquad_member(y, e);
					tquad_to_string(tquad(e, 0xffffffff), es, 33);
					printf("%s %5s %5s\n", es, x_memb ? "true" : "false", y_memb ? "true" : "false");

					if (tquad_member(x, e)) {
						assert(tquad_member(y, e));
					} else {
						assert(!tquad_member(y, e));
					}
				}
				break;
			case RELATION_SUBSET:
				printf("%-12s: ", "subset");

				tquad_to_string(x, xs, 33);
				assert(tquad_from_string(xs, &tmp));
				assert(tquad_equal(x, tmp));

				tquad_to_string(y, ys, 33);
				assert(tquad_from_string(ys, &tmp));
				assert(tquad_equal(y, tmp));

				printf("%s\n%12s  %s\n\n", xs, "", ys);

				y_only = false;
				for (e = 0; e < (uint32_t)(1 << width); e++) {

					x_memb = tquad_member(x, e);
					y_memb = tquad_member(y, e);
					tquad_to_string(tquad(e, 0xffffffff), es, 33);
					printf("%s %5s %5s\n", es, x_memb ? "true" : "false", y_memb ? "true" : "false");

					if (tquad_member(x, e)) {
						assert(tquad_member(y, e));
					} else if (!y_only) {
						if (tquad_member(y, e)) y_only = true;
					}
				}
				assert(y_only);
				break;
			case RELATION_SUPERSET:
				printf("%-12s: ", "superset");

				tquad_to_string(x, xs, 33);
				assert(tquad_from_string(xs, &tmp));
				assert(tquad_equal(x, tmp));

				tquad_to_string(y, ys, 33);
				assert(tquad_from_string(ys, &tmp));
				assert(tquad_equal(y, tmp));

				printf("%s\n%12s  %s\n\n", xs, "", ys);

				x_only = false;
				for (e = 0; e < (uint32_t)(1 << width); e++) {

					x_memb = tquad_member(x, e);
					y_memb = tquad_member(y, e);
					tquad_to_string(tquad(e, 0xffffffff), es, 33);
					printf("%s %5s %5s\n", es, x_memb ? "true" : "false", y_memb ? "true" : "false");

					if (tquad_member(y, e)) {
						assert(tquad_member(x, e));
					} else if (!x_only) {
						if (tquad_member(x, e)) x_only = true;
					}
				}
				assert(x_only);
				break;
			case RELATION_INTERSECT:
				printf("%-12s: ", "intersect");

				tquad_to_string(x, xs, 33);
				assert(tquad_from_string(xs, &tmp));
				assert(tquad_equal(x, tmp));

				tquad_to_string(y, ys, 33);
				assert(tquad_from_string(ys, &tmp));
				assert(tquad_equal(y, tmp));

				printf("%s\n%12s  %s\n\n", xs, "", ys);

				x_only = y_only = x_and_y = false;
				for (e = 0; e < (uint32_t)(1 << width); e++) {

					x_memb = tquad_member(x, e);
					y_memb = tquad_member(y, e);
					tquad_to_string(tquad(e, 0xffffffff), es, 33);
					printf("%s %5s %5s\n", es, x_memb ? "true" : "false", y_memb ? "true" : "false");

					if (tquad_member(x, e)) {
						if (tquad_member(y, e)) {
							x_and_y = true;
						} else {
							x_only = true;
						}
					} else if (tquad_member(y, e)) {
						y_only = true;
					}
				}
				assert(x_only);
				assert(y_only);
				assert(x_and_y);
				break;
			case RELATION_DISJOINT:
				printf("%-12s: ", "disjoint");

				tquad_to_string(x, xs, 33);
				assert(tquad_from_string(xs, &tmp));
				assert(tquad_equal(x, tmp));

				tquad_to_string(y, ys, 33);
				assert(tquad_from_string(ys, &tmp));
				assert(tquad_equal(y, tmp));

				printf("%s\n%12s  %s\n\n", xs, "", ys);

				for (e = 0; e < (uint32_t)(1 << width); e++) {

					x_memb = tquad_member(x, e);
					y_memb = tquad_member(y, e);
					tquad_to_string(tquad(e, 0xffffffff), es, 33);
					printf("%s %5s %5s\n", es, x_memb ? "true" : "false", y_memb ? "true" : "false");

					assert(!(tquad_member(x, e) && tquad_member(y, e)));
				}
				break;
			default:
				/* *** NOT REACHED *** */
				assert(0);
		}
		getchar();
	}
}

	int
main(int argc, char *argv[])
{
	tquad_t x, y;
	tbit_t x0, x1, y0, y1;
	char xs[33], ys[33];

	selftest(4);

	for (x1 = TBIT0;; x1++) {
		for (x0 = TBIT0;; x0++) {
			for (y1 = TBIT0;; y1++) {
				for (y0 = TBIT0;; y0++) {
					x = tquad(0x00000000, 0x00000000);
					tquad_put(&x, x0, 0);
					tquad_put(&x, x1, 1);
					tquad_to_string(x, xs, 33);

					y = tquad(0x00000000, 0x00000000);
					tquad_put(&y, y0, 0);
					tquad_put(&y, y1, 1);
					tquad_to_string(y, ys, 33);

					printf("%s\n%s\n", xs, ys);

					switch(tquad_relation(x, y)) {
						case RELATION_EQUAL:
							printf("equal\n\n");
							break;
						case RELATION_SUBSET:
							printf("subset\n\n");
							break;
						case RELATION_SUPERSET:
							printf("superset\n\n");
							break;
						case RELATION_INTERSECT:
							printf("intersect\n\n");
							break;
						case RELATION_DISJOINT:
							printf("disjoint\n\n");
							break;
						default:
							/* *** NOT REACHED *** */
							assert(0);
					}

					getchar();

					if (y0 == TBIT_) break;
				}
				if (y1 == TBIT_) break;
			}
			if (x0 == TBIT_) break;
		}
		if (x1 == TBIT_) break;
	}
}
