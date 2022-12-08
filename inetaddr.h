/**
 * @file inetaddr.h
 * @brief IP address utilities.
 * @author Johan Rönnberg <johan@fabinv.com>
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
#ifndef INETADDR
#define INETADDR

#include <arpa/inet.h>

#include <stdint.h>

/** IP address */
struct inetaddr {
	union {
		uint8_t addr8[16];
		uint32_t addr32[4];
	} s;
};
typedef struct inetaddr inetaddr_t;

/* Extract the raw IPv4 address from an ipaddr_t */
#define IA_V4(iaddr) (&(iaddr)->s.addr32[3])

/* Check if ipaddr is an "IPv4-mapped IPv6 address" (see RFC 2373). */
#define IS_IA_V4MAPPED_V6 IS_IA_V4
#define IS_IA_V4(iaddr)                                                        \
	((iaddr)->s.addr32[0] == 0 &&                                              \
	 (iaddr)->s.addr32[1] == 0 &&                                              \
	 (iaddr)->s.addr32[2] == htonl(0x0000FFFF))

/* Check if ipaddr is an "IPv4-compatible IPv6 address" (see RFC 2373). */
#define IS_IA_V4COMPAT_V6(iaddr)                                               \
	((iaddr)->s.addr32[0] == 0 &&                                              \
	 (iaddr)->s.addr32[1] == 0 &&                                              \
	 (iaddr)->s.addr32[2] == 0)

/* Put a 32 bit IPv4 address into an ipaddr as a IPv4-mapped IPv6 address */
#define IA_V4_TO_IADDR(iaddr, iav4)                                            \
	do {                                                                       \
		(iaddr)->s.addr32[0] = (iaddr)->s.addr32[1] = 0;                       \
		(iaddr)->s.addr32[2] = htonl(0x0000FFFF);                              \
		memcpy(&(iaddr)->s.addr32[3], iav4, 4);                                \
	} while (0)

/* Put a 128 bit raw IPv6 address into an ipaddr */
#define IA_V6_TO_IADDR(iaddr, iav6) memcpy(iaddr, iav6, 16)

#define PREFIX_LEN_TO_IADDRMASK(iaddr, mlen)                                    \
	do {                                                                        \
		memset(iaddr, 0x00, 16);                                                \
		memset(iaddr, 0xFF, (mlen) >> 3);                                       \
		if ((mlen) >> 3 < 16) {                                                 \
			(iaddr)->s.addr8[(mlen) >> 3] = ~(0xFF >> ((mlen) & 0x7));          \
		}                                                                       \
	} while (0)

#endif
