/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// Long ago this file contained code from Arduino.cc, which was
// Copyright (c) 2008 David A. Mellis.  No substantial portion of
// Arduino's original code remains.  In fact, several improvements
// developed for Teensyduino have made their way back into
// Arduino's code base.  :-)

// Include our own class declaration unconditionally so the definitions
// below compile regardless of whether Arduino.h transitively provides it.
// Print.h pulls in WString.h (String) and Printable.h.
#include "Print.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>

#ifndef __IMXRT1176_HOST_TEST__
#include <Arduino.h>
#endif

size_t Print::write(const uint8_t *buffer, size_t size)
{
	if (buffer == nullptr) return 0;
	size_t count = 0;
	while (size--) count += write(*buffer++);
	return count;
}


size_t Print::print(const String &s)
{
	uint8_t buffer[33];
	size_t count = 0;
	unsigned int index = 0;
	unsigned int len = s.length();
	while (len > 0) {
		s.getBytes(buffer, sizeof(buffer), index);
		unsigned int nbytes = len;
		if (nbytes > sizeof(buffer)-1) nbytes = sizeof(buffer)-1;
		index += nbytes;
		len -= nbytes;
		count += write(buffer, nbytes);
	}
	return count;
}

size_t Print::print(long n)
{
	uint8_t sign=0;

	if (n < 0) {
		sign = '-';
		n = -n;
	}
	return printNumber(n, 10, sign);
}

size_t Print::print(int64_t n)
{
	if (n < 0) return printNumber64(-n, 10, 1);
	return printNumber64(n, 10, 0);
}

size_t Print::println(void)
{
	uint8_t buf[2]={'\r', '\n'};
	return write(buf, 2);
}

// The teensy4 core routed printf through a newlib _write() hook that
// resolved to &Serial. This minimal core has no Serial yet (Phase 1) and
// no debug/printf.h, so we implement printf portably: format into a fixed
// stack buffer with vsnprintf, then write() the bytes. Works on both ARM
// and host with no external dependency.
// NOTE: output is limited to PRINTF_BUF_SIZE-1 bytes per call; longer
// results are truncated (acceptable for Phase 1).
#define PRINTF_BUF_SIZE 128

int Print::printf(const char *format, ...)
{
	char buf[PRINTF_BUF_SIZE];
	va_list ap;
	va_start(ap, format);
	int len = vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	if (len < 0) return 0;
	// vsnprintf returns the length it WOULD have written; clamp to what fit.
	size_t n = (len < (int)sizeof(buf)) ? (size_t)len : sizeof(buf) - 1;
	return write((const uint8_t *)buf, n);
}

int Print::printf(const __FlashStringHelper *format, ...)
{
	char buf[PRINTF_BUF_SIZE];
	va_list ap;
	va_start(ap, format);
	int len = vsnprintf(buf, sizeof(buf), (const char *)format, ap);
	va_end(ap);
	if (len < 0) return 0;
	size_t n = (len < (int)sizeof(buf)) ? (size_t)len : sizeof(buf) - 1;
	return write((const uint8_t *)buf, n);
}

int Print::vprintf(const char *format, va_list ap)
{
	char buf[PRINTF_BUF_SIZE];
	int len = vsnprintf(buf, sizeof(buf), format, ap);
	if (len < 0) return 0;
	size_t n = (len < (int)sizeof(buf)) ? (size_t)len : sizeof(buf) - 1;
	return write((const uint8_t *)buf, n);
}

size_t Print::printNumber(unsigned long n, uint8_t base, uint8_t sign)
{
	uint8_t buf[34];
	uint8_t digit, i;

	// TODO: make these checks as inline, since base is
	// almost always a constant.  base = 0 (BYTE) should
	// inline as a call directly to write()
	if (base == 0) {
		return write((uint8_t)n);
	} else if (base == 1) {
		base = 10;
	}


	if (n == 0) {
		buf[sizeof(buf) - 1] = '0';
		i = sizeof(buf) - 1;
	} else {
		i = sizeof(buf) - 1;
		while (1) {
			digit = n % base;
			buf[i] = ((digit < 10) ? '0' + digit : 'A' + digit - 10);
			n /= base;
			if (n == 0) break;
			i--;
		}
	}
	if (sign) {
		i--;
		buf[i] = '-';
	}
	return write(buf + i, sizeof(buf) - i);
}

size_t Print::printNumber64(uint64_t n, uint8_t base, uint8_t sign)
{
	uint8_t buf[66];
	uint8_t digit, i;

	if (base < 2) return 0;
	if (n == 0) {
		buf[sizeof(buf) - 1] = '0';
		i = sizeof(buf) - 1;
	} else {
		i = sizeof(buf) - 1;
		while (1) {
			digit = n % base;
			buf[i] = ((digit < 10) ? '0' + digit : 'A' + digit - 10);
			n /= base;
			if (n == 0) break;
			i--;
		}
	}
	if (sign) {
		i--;
		buf[i] = '-';
	}
	return write(buf + i, sizeof(buf) - i);
}

size_t Print::printFloat(double number, uint8_t digits) 
{
	uint8_t sign=0;
	size_t count=0;

	if (isnan(number)) return print("nan");
    	if (isinf(number)) return print("inf");
    	if (number > 4294967040.0f) return print("ovf");  // constant determined empirically
    	if (number <-4294967040.0f) return print("ovf");  // constant determined empirically
	
	// Handle negative numbers
	if (number < 0.0) {
		sign = 1;
		number = -number;
	}

	// Round correctly so that print(1.999, 2) prints as "2.00"
	double rounding = 0.5;
	for (uint8_t i=0; i<digits; ++i) {
		rounding *= 0.1;
	}
	number += rounding;

	// Extract the integer part of the number and print it
	unsigned long int_part = (unsigned long)number;
	double remainder = number - (double)int_part;
	count += printNumber(int_part, 10, sign);

	// Print the decimal point, but only if there are digits beyond
	if (digits > 0) {
		uint8_t n, buf[16], count=1;
		buf[0] = '.';

		// Extract digits from the remainder one at a time
		if (digits > sizeof(buf) - 1) digits = sizeof(buf) - 1;

		while (digits-- > 0) {
			remainder *= 10.0;
			n = (uint8_t)(remainder);
			buf[count++] = '0' + n;
			remainder -= n; 
		}
		count += write(buf, count);
	}
	return count;
}


