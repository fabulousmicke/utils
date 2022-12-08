/**
 * @file progress.h
 * @brief Progress reporting
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
#ifndef PROGRESS
#define PROGRESS() PROGRESS_CHAR('.')

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bench.h>

#define PROGRESS_DEPTH_MAX 256
#define PROGRESS_DEPTH_LIM 4

#if PROGRESS_DEPTH_LIM > PROGRESS_DEPTH_MAX
#error "Progress depth limit may not exceed progress max depth\n");
#endif

#define PROGRESS_DOTS ((size_t)400)

#define PROGRESS_WRAP ((size_t)100)

#define PROGRESS_TASK_LEN ((size_t)65)

#define PROGRESS_INIT_FORMAT "%-32s "

#define PROGRESS_INDENT "    "

#define PROGRESS_SHOW(...)                                                \
{                                                                     \
	if (progress_stack.o || progress_stack.d <= PROGRESS_DEPTH_LIM) { \
		fprintf(stderr, ##__VA_ARGS__);                               \
	}                                                                 \
}

#if 0
/* This is maybe TODO if we want to control timing units and turn off timing. May be overkill. */
typedef enum {
	PROGRESS_UNIT_AUTO,
	PROGRESS_UNIT_NANOSECONDS,
	PROGRESS_UNIT_MICROSECONDS,
	PROGRESS_UNIT_MILLISECONDS,
	PROGRESS_UNIT_SECONDS,
	PROGRESS_UNIT_NO_TIMING
} progress_unit_t;
#endif

struct progress {
	char t[PROGRESS_TASK_LEN];
	uint64_t s;
	uint64_t f;
	size_t n;
	size_t m;
	size_t k;
	size_t i;
	size_t j;
	bool e;
};
typedef struct progress progress_t;

struct progress_stack {
	progress_t p[PROGRESS_DEPTH_MAX];
	size_t d;
	bool o;
};
typedef struct progress_stack progress_stack_t;

extern progress_stack_t progress_stack;

	static inline void
progress_newline(void)
{
	PROGRESS_SHOW("\n");
	for (size_t i = 1; i < progress_stack.d; i++) {
		PROGRESS_SHOW(PROGRESS_INDENT);
	}
	progress_stack.p[progress_stack.d - 1].j = 0;
	progress_stack.p[progress_stack.d - 1].e = false;
}

	static inline void
progress_init(
		const char *t,
		size_t n,
		size_t m)
{
	if (progress_stack.d >= PROGRESS_DEPTH_MAX) {
		PROGRESS_SHOW("\nPROGRESS_INIT* called when progress stack is full - aborting!\n");
		exit(1);
	}
	progress_stack.p[progress_stack.d].k = SIZE_MAX;
	if (((n == 0) && (m == 0))) {
		progress_stack.p[progress_stack.d].n = 0;
		progress_stack.p[progress_stack.d].m = 1;
		progress_stack.p[progress_stack.d].k = 1;
	} else if (m == 0) {
		m = n / PROGRESS_DOTS;
		m += (size_t)(m == 0);
		progress_stack.p[progress_stack.d].m = m;
	} else {
		progress_stack.p[progress_stack.d].m = m;
	}
	if (progress_stack.p[progress_stack.d].j > 0) {
		progress_stack.p[progress_stack.d].j = PROGRESS_WRAP;
	}
	progress_stack.p[progress_stack.d].i = 0;
	progress_stack.p[progress_stack.d].e = false;
	strncpy(progress_stack.p[progress_stack.d].t, t, PROGRESS_TASK_LEN);
	progress_stack.p[progress_stack.d].s = bench_nanoseconds();
	progress_stack.d++;

	progress_newline();
}

	static inline void
progress_exit_pre(
		const char *file,
		int line)
{
	if (progress_stack.d == 0) {
		fprintf(stderr, "\nPROGRESS_EXIT* called when progress stack is empty - aborting!\n");
		fprintf(stderr, "Called from \"%s\", line %d\n", file, line);
		exit(1);
	}
	progress_stack.p[progress_stack.d - 1].f = bench_nanoseconds();
	if (progress_stack.p[progress_stack.d - 1].j > 0) {
		progress_newline();
	} else if (progress_stack.p[progress_stack.d - 1].e) {
		progress_newline();
	}
}

	static inline void
progress_exit_post(void)
{
	progress_newline();
	progress_stack.d--;
	if (progress_stack.d > 0) progress_stack.p[progress_stack.d - 1].e = true;
}

	static inline void
progress_char(char c)
{
	progress_stack.p[progress_stack.d - 1].i++;

	if (progress_stack.p[progress_stack.d - 1].k < SIZE_MAX) {
		if (progress_stack.p[progress_stack.d - 1].i & ((1LU << progress_stack.p[progress_stack.d - 1].k) - 1UL)) return;
	}
	if ((progress_stack.p[progress_stack.d - 1].i % progress_stack.p[progress_stack.d - 1].m) == 0) {
		if (progress_stack.p[progress_stack.d - 1].j == PROGRESS_WRAP) {
			progress_stack.p[progress_stack.d - 1].j = 0;
			if (progress_stack.p[progress_stack.d - 1].k < SIZE_MAX) {
				progress_stack.p[progress_stack.d - 1].k++;
			}
			progress_newline();
		}
		assert(progress_stack.p[progress_stack.d - 1].j < PROGRESS_WRAP);
		PROGRESS_SHOW("%c", c);
		progress_stack.p[progress_stack.d - 1].j++;
	}
}

	static inline void
progress_text(void)
{
	if (progress_stack.p[progress_stack.d - 1].j > 0) {
		progress_newline();
	} else if (progress_stack.p[progress_stack.d - 1].e) {
		progress_newline();
	}
}

	static inline uint64_t
progress_elap(void)
{
	uint64_t result;

	result = progress_stack.p[progress_stack.d - 1].f - progress_stack.p[progress_stack.d - 1].s;

#ifdef PROGRESS_FIXED_UNIT
	return result;
#else
	if (result < 1000) return result; /* nanoseconds */
	result /= 1000;
	if (result < 1000) return result; /* microseconds */
	result /= 1000;
	if (result < 1000) return result; /* milliseconds */
	result /= 1000;
	return result; /* seconds */
#endif
}

	static inline const char *
progress_unit(void)
{
#ifdef PROGRESS_FIXED_UNIT
	return "nanoseconds";
#else
	uint64_t e = progress_stack.p[progress_stack.d - 1].f - progress_stack.p[progress_stack.d - 1].s;

	if (e < 1000) return "nanoseconds";
	e /= 1000;
	if (e < 1000) return "microseconds";
	e /= 1000;
	if (e < 1000) return "milliseconds";
	return "seconds";
#endif
}

	static inline const char *
progress_task(void)
{
	return progress_stack.p[progress_stack.d - 1].t;
}

#ifndef NO_PROGRESS
#define PROGRESS_INIT(n, m, t)                      \
	do {                                            \
		progress_init(t, (size_t)(n), (size_t)(m)); \
		PROGRESS_SHOW("%s", t);                     \
		progress_newline();                         \
	} while (0);

#define PROGRESS_INIT_TEXT(n, m, t, f, ...)         \
	do {                                            \
		progress_init(t, (size_t)(n), (size_t)(m)); \
		PROGRESS_SHOW("%s", t);                     \
		PROGRESS_SHOW(f, ##__VA_ARGS__);            \
		progress_newline();                         \
	} while (0);

#define PROGRESS_TEXT(f, ...)            \
	do {                                 \
		progress_text();                 \
		PROGRESS_SHOW(f, ##__VA_ARGS__); \
		progress_newline();              \
	} while (0)

#define PROGRESS_CHAR(c) progress_char(c)

#define PROGRESS_EXIT()                                         \
	do {                                                        \
		progress_exit_pre(__FILE__, __LINE__);                  \
		PROGRESS_SHOW("%s completed in %" PRIu64 " %s",         \
				progress_task(), progress_elap(), progress_unit()); \
		progress_exit_post();                                   \
	} while (0)

#define PROGRESS_EXIT_TEXT(f, ...)                    \
	do {                                              \
		progress_exit_pre(__FILE__, __LINE__);        \
		PROGRESS_SHOW("%s",                           \
				progress_task());                         \
		PROGRESS_SHOW(f, ##__VA_ARGS__);              \
		PROGRESS_SHOW(" completed in %" PRIu64 " %s", \
				progress_elap(), progress_unit());        \
		progress_exit_post();                         \
	} while (0)
#else
#define PROGRESS_INIT(n, m, t) \
	do {                       \
	} while (0);

#define PROGRESS_INIT_TEXT(n, m, t, f, ...) \
	do {                                    \
	} while (0);

#define PROGRESS_TEXT(f, ...) \
	do {                      \
	} while (0)

#define PROGRESS_CHAR(c) \
	do {                 \
	} while (0)

#define PROGRESS_EXIT() \
	do {                \
	} while (0)

#define PROGRESS_EXIT_TEXT(f, ...) \
	do {                           \
	} while (0)
#endif

#define PROGRESS_OVERRIDE_ON() progress_stack.o = true;
#define PROGRESS_OVERRIDE_OFF() progress_stack.o = false;
#endif /* PROGRESS */
