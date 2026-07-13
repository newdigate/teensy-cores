/* nonstd.c - AVR-libc-style number formatting (ultoa/ltoa/ulltoa/lltoa/
 * dtostrf) for the imxrt1176 core.
 *
 * Clean-room MIT implementation: written from the prototypes declared in
 * avr_functions.h and the documented AVR-libc semantics (radix 2-36 digit
 * conversion; dtostrf right-aligns into a minimum field width with fixed
 * decimals). Not derived from any LGPL Teensyduino source. These symbols
 * were declared but never defined in this core until the 2026-07-13 license
 * clean-room pass added them (String's numeric constructors reference them).
 *
 * Copyright (c) 2026 Nicholas Newdigate
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <string.h>

static const char digit_chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";

// Shared core: emit val in the given radix (2-36) into buf, return buf.
// Digits are generated least-significant-first into a scratch area at the
// end of the caller-supplied buffer region, then the string is the tail.
static char *format_unsigned(unsigned long long val, char *buf, int radix,
                             int negative)
{
	char tmp[64 + 2];          // 64 binary digits + sign + NUL worst case
	char *p = tmp + sizeof(tmp) - 1;

	if (radix < 2 || radix > 36) {
		buf[0] = 0;
		return buf;
	}
	*p = 0;
	do {
		*--p = digit_chars[val % (unsigned)radix];
		val /= (unsigned)radix;
	} while (val);
	if (negative)
		*--p = '-';
	return strcpy(buf, p);
}

char *ultoa(unsigned long val, char *buf, int radix)
{
	return format_unsigned(val, buf, radix, 0);
}

char *ltoa(long val, char *buf, int radix)
{
	// Negative values are rendered with '-' only in base 10 (AVR-libc
	// renders other radixes as the unsigned bit pattern).
	if (radix == 10 && val < 0)
		return format_unsigned(0ULL - (unsigned long long)(long long)val,
		                       buf, radix, 1);
	return format_unsigned((unsigned long)val, buf, radix, 0);
}

char *ulltoa(unsigned long long val, char *buf, int radix)
{
	return format_unsigned(val, buf, radix, 0);
}

char *lltoa(long long val, char *buf, int radix)
{
	if (radix == 10 && val < 0)
		return format_unsigned(0ULL - (unsigned long long)val, buf, radix, 1);
	return format_unsigned((unsigned long long)val, buf, radix, 0);
}

// dtostrf: fixed-point decimal formatting. 'width' is the MINIMUM total
// field width (right-aligned, space-padded, negative width = left-aligned);
// 'precision' is the exact number of digits after the decimal point.
char *dtostrf(float val, int width, unsigned int precision, char *buf)
{
	char *out = buf;
	int left_align = 0;
	unsigned int i;

	if (width < 0) {
		left_align = 1;
		width = -width;
	}

	// Round half away from zero at the requested precision.
	double d = val;
	int negative = 0;
	if (d < 0) {
		negative = 1;
		d = -d;
	}
	double round = 0.5;
	for (i = 0; i < precision; i++)
		round /= 10.0;
	d += round;

	unsigned long long int_part = (unsigned long long)d;
	double frac = d - (double)int_part;

	char body[64 + 2];
	char *p = body;
	if (negative)
		*p++ = '-';
	ulltoa(int_part, p, 10);
	p += strlen(p);
	if (precision > 0) {
		*p++ = '.';
		for (i = 0; i < precision && (size_t)(p - body) < sizeof(body) - 1; i++) {
			frac *= 10.0;
			int digit = (int)frac;
			*p++ = (char)('0' + digit);
			frac -= digit;
		}
	}
	*p = 0;

	int len = (int)(p - body);
	int pad = width - len;
	if (pad < 0)
		pad = 0;
	if (!left_align)
		while (pad-- > 0)
			*out++ = ' ';
	strcpy(out, body);
	out += len;
	if (left_align)
		while (pad-- > 0)
			*out++ = ' ';
	*out = 0;
	return buf;
}
