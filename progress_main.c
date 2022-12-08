/**
 * @file progress_main.c
 * @brief Progress reporting testing
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
 *
 */
#include <progress.h>

	int
main(
		int argc,
		char *argv[])
{
	int n, m;

	PROGRESS_INIT(0, 0, "auto dots");
	n = (1 << 30) - 1;
	for (int i = 0; i < n; i++) {
		PROGRESS();
	}
	PROGRESS_EXIT();

	m = 0;
	for (n = 100000; n > 0; n /= 10) {
		PROGRESS_INIT(n, 0, "capped number of dots");
		for (int i = 0; i < n; i++) {
			PROGRESS_CHAR('.');
		}
		PROGRESS_TEXT("n = %d, m = 0 => ~%zu dots", n, PROGRESS_DOTS);
		PROGRESS_EXIT();
	}

	m = 1000;
	for (n = 10000; n > 0; n /= 10) {
		PROGRESS_INIT(n, m, "linear number of dots");
		for (int i = 0; i < n; i++) {
			PROGRESS_CHAR('.');
		}
		PROGRESS_TEXT("n = %d, m = %d => ~%d dots", n, m, n / m);
		PROGRESS_EXIT();
	}

	m = 0;
	for (n = 1000; n > 0; n /= 10) {
		PROGRESS_INIT(0, 0, "all dots");
		for (int i = 0; i < n; i++) {
			PROGRESS_CHAR('.');
		}
		PROGRESS_TEXT("n = 0, m = 0 => ~%d dots", n);
		PROGRESS_EXIT();
	}

	n = 100000;
	PROGRESS_INIT(n, 0, "super task");
	for (int i = 0; i < n; i++) {
		PROGRESS_CHAR('.');
		if (i > 0 && (i % (n / 5)) == 0) {
			PROGRESS_INIT(20000, 0, "sub task");
			for (int j = 0; j < 20000; j++) {
				PROGRESS_CHAR('.');
			}
			PROGRESS_EXIT();
		}
	}
	PROGRESS_EXIT();

	PROGRESS_INIT_TEXT(0, 0, "no dots", ", that is %d dots!, but you have to hit ENTER", 0);
	getchar();
	PROGRESS_EXIT();

	PROGRESS_INIT(0, 0, "no dots super task");
	PROGRESS_INIT(0, 0, "no dots sub task");
	PROGRESS_EXIT();
	PROGRESS_INIT(0, 0, "all dots sub task");
	for (int i = 0; i < 267; i++)
		PROGRESS_CHAR('.');
	PROGRESS_EXIT();
	PROGRESS_EXIT();

	PROGRESS_INIT(0, 0, "no dots super task");
	PROGRESS_INIT(0, 0, "all dots sub task");
	for (int i = 0; i < 267; i++)
		PROGRESS_CHAR('.');
	PROGRESS_EXIT();
	PROGRESS_INIT(0, 0, "no dots sub task");
	PROGRESS_TEXT("no dots sub task text");
	PROGRESS_EXIT();
	PROGRESS_TEXT("no dots super task text");
	PROGRESS_EXIT();

	PROGRESS_INIT(0, 0, "all dots super task");
	for (int i = 0; i < 136; i++)
		PROGRESS_CHAR('.');
	PROGRESS_INIT(0, 0, "no dots sub task");
	PROGRESS_EXIT();
	for (int i = 0; i < 372; i++)
		PROGRESS_CHAR('.');
	PROGRESS_INIT(0, 0, "all dots sub task");
	for (int i = 0; i < 147; i++)
		PROGRESS_CHAR('.');
	PROGRESS_EXIT();
	for (int i = 0; i < 274; i++)
		PROGRESS_CHAR('.');
	PROGRESS_TEXT("all dots super task text");
	for (int i = 0; i < 39; i++)
		PROGRESS_CHAR('.');
	PROGRESS_EXIT_TEXT(", and that is all (i.e. %d tasks left)", 0);

#ifdef PROGRESS_OVERFLOW_TEST
	for (int i; i < 1000; i++) {
		char t[1024];

		sprintf(t, "task level %d", i);
		PROGRESS_INIT(0, 0, t);
	}
#endif

#ifdef PROGRESS_UNDERFLOW_TEST
	PROGRESS_EXIT();
#endif

	return 0;
}
