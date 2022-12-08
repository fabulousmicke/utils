/**
 * @file journal.h
 * @brief Utilities for journaling and tracing.
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
 */
#ifndef JOURNAL
#define JOURNAL

// TODO Implement support for journaling to file for post morten analysis.

#define JOURNAL_STREAM stderr

#define JOURNAL_PRINTF(format, ...) do {if (1) { fprintf(stderr, "File \"%s\", function \"%s\", line %d: \n", __FILE__, __func__, __LINE__); fprintf(stderr, format, ##__VA_ARGS__);}}  while (0)

#define JOURNAL_LOG(format, ...) do {if (1) { fprintf(stderr, format, ##__VA_ARGS__);}}  while (0)

#endif
