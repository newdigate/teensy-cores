/* Stream.cpp - implementation of the Stream timeout/parse/find helper API
 * for the imxrt1176 core.
 *
 * Clean-room MIT implementation: written from the documented Arduino Stream
 * API surface and this core's extracted interface contract, not derived
 * from the LGPL Arduino/Teensyduino Stream sources.
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

#ifdef __IMXRT1176_HOST_TEST__
// Host self-test build: no Arduino.h; local monotonic millis() + no-op yield()
#include "Stream.h"
#include <string.h>
#include <time.h>
static unsigned long millis(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (unsigned long)ts.tv_sec * 1000UL + (unsigned long)(ts.tv_nsec / 1000000L);
}
static void yield(void) {}
#else
#include "Arduino.h"
#include <string.h>
#endif

// read() with timeout: retry until a byte arrives or _timeout ms elapse.
// A timeout of 0 still attempts one read.
int Stream::timedRead()
{
	unsigned long start = millis();
	do {
		int c = read();
		if (c >= 0) return c;
		yield();
	} while (millis() - start < _timeout);
	return -1;	// timed out
}

// peek() with timeout: as timedRead(), but does not consume the byte
int Stream::timedPeek()
{
	unsigned long start = millis();
	do {
		int c = peek();
		if (c >= 0) return c;
		yield();
	} while (millis() - start < _timeout);
	return -1;	// timed out
}

// peek (with timeout) at the next character that could start a number,
// consuming lead-in characters according to the lookahead mode.
// Returns the numeric lead character (not consumed), or -1.
int Stream::peekNextDigit(LookaheadMode lookahead, bool detectDecimal)
{
	while (1) {
		int c = timedPeek();
		if (c < 0) return -1;	// timed out
		if (c == '-') return c;
		if (c >= '0' && c <= '9') return c;
		if (detectDecimal && c == '.') return c;
		switch (lookahead) {
		case SKIP_NONE:
			return -1;
		case SKIP_WHITESPACE:
			if (c != ' ' && c != '\t' && c != '\r' && c != '\n') return -1;
			break;
		case SKIP_ALL:
			break;
		}
		read();	// consume the non-numeric character, keep looking
	}
}

// parse a long: optional leading '-', decimal digits; 'ignore' chars are
// skipped inside the number.  Returns 0 on timeout / nothing numeric.
// The first character that is neither a digit nor 'ignore' is not consumed.
long Stream::parseInt(LookaheadMode lookahead, char ignore)
{
	bool negative = false;
	unsigned long value = 0;	// unsigned: overflow wraps, no UB
	int c = peekNextDigit(lookahead, false);
	if (c < 0) return 0;	// timeout / no numeric lead-in
	do {
		if (c >= 0 && (char)c == ignore) {
			// skip, do not accumulate
		} else if (c == '-') {
			negative = true;
		} else {
			value = value * 10 + (unsigned long)(c - '0');
		}
		read();	// consume the accepted character
		c = timedPeek();
	} while ((c >= '0' && c <= '9') || (c >= 0 && (char)c == ignore));
	return negative ? (long)(0UL - value) : (long)value;
}

// parse a float: as parseInt, plus at most one '.' followed by a decimal
// fraction.  A second '.' terminates parsing.
float Stream::parseFloat(LookaheadMode lookahead, char ignore)
{
	bool negative = false;
	bool isFraction = false;
	unsigned long value = 0;	// unsigned: overflow wraps, no UB
	float fraction = 1.0f;
	int c = peekNextDigit(lookahead, true);
	if (c < 0) return 0;	// timeout / no numeric lead-in
	do {
		if (c >= 0 && (char)c == ignore) {
			// skip, do not accumulate
		} else if (c == '-') {
			negative = true;
		} else if (c == '.') {
			isFraction = true;
		} else {
			value = value * 10 + (unsigned long)(c - '0');
			if (isFraction) fraction *= 0.1f;
		}
		read();	// consume the accepted character
		c = timedPeek();
	} while ((c >= '0' && c <= '9') || (c >= 0 && (char)c == ignore)
		|| (c == '.' && !isFraction));
	float result = (float)value * (isFraction ? fraction : 1.0f);
	return negative ? -result : result;
}

bool Stream::findUntil(const char *target, const char *terminator)
{
	if (target == NULL) return true;
	return findUntil(target, strlen(target), terminator,
		terminator ? strlen(terminator) : 0);
}

// Advance a running match of pattern[0..len-1] (currently matched to depth
// index, index < len) by one input character c; returns the new depth.
// On mismatch, fall back to the longest shorter resume depth that is
// consistent with the input consumed so far: since the consumed suffix
// equals pattern[0..index-1], this is a KMP-style failure computation done
// by self-comparison of the pattern, in constant memory.
static size_t match_advance(const char *pattern, size_t index, char c)
{
	if (c == pattern[index]) return index + 1;
	for (size_t k = index; k >= 1; k--) {
		if (pattern[k - 1] != c) continue;
		bool consistent = true;
		for (size_t i = 0; i + 1 < k; i++) {
			if (pattern[i] != pattern[index - k + 1 + i]) {
				consistent = false;
				break;
			}
		}
		if (consistent) return k;
	}
	return 0;
}

// consume the stream, tracking a running match against target (and, in
// parallel, terminate).  True when target completes; false when the
// terminator completes first, on timeout, or on a byte value <= 0
// (0 is treated as end-of-data).
bool Stream::findUntil(const char *target, size_t targetLen, const char *terminate, size_t termLen)
{
	if (target == NULL || targetLen == 0 || *target == 0) return true;
	if (terminate == NULL) termLen = 0;
	size_t index = 0;
	size_t termIndex = 0;
	int c;
	while ((c = timedRead()) > 0) {
		index = match_advance(target, index, (char)c);
		if (index >= targetLen) return true;
		if (termLen > 0) {
			termIndex = match_advance(terminate, termIndex, (char)c);
			if (termIndex >= termLen) return false;
		}
	}
	return false;
}

// read up to length bytes with per-byte timeout; buffer is NOT null
// terminated.  Timeout sets the read-error flag and stops early.
size_t Stream::readBytes(char *buffer, size_t length)
{
	if (buffer == NULL) return 0;
	size_t count = 0;
	while (count < length) {
		int c = timedRead();
		if (c < 0) {
			setReadError();
			break;
		}
		buffer[count++] = (char)c;
	}
	return count;
}

// as readBytes, but stop at the terminator (consumed, not stored); at
// most length-1 characters stored, always null terminated.  Returns the
// count excluding the null.
size_t Stream::readBytesUntil(char terminator, char *buffer, size_t length)
{
	if (buffer == NULL || length < 1) return 0;
	size_t count = 0;
	while (count < length - 1) {
		int c = timedRead();
		if (c < 0) {
			setReadError();
			break;
		}
		if ((char)c == terminator) break;
		buffer[count++] = (char)c;
	}
	buffer[count] = 0;
	return count;
}

// accumulate into a String until timeout (sets the read-error flag), a
// NUL byte, or max characters (max == 0 means unlimited)
String Stream::readString(size_t max)
{
	String str;
	if (max != 0) str.reserve(max);
	size_t count = 0;
	while (max == 0 || count < max) {
		int c = timedRead();
		if (c < 0) {
			setReadError();
			break;
		}
		if (c == 0) break;
		// grow in chunks rather than per byte when unbounded
		if (max == 0 && (count & 63) == 0) str.reserve(count + 64);
		size_t before = str.length();
		str += (char)c;
		if (str.length() == before) {	// append failed: out of memory
			setReadError();
			break;
		}
		count++;
	}
	return str;
}

// as readString, but also stop at the terminator (consumed, not appended)
String Stream::readStringUntil(char terminator, size_t max)
{
	String str;
	if (max != 0) str.reserve(max);
	size_t count = 0;
	while (max == 0 || count < max) {
		int c = timedRead();
		if (c < 0) {
			setReadError();
			break;
		}
		if (c == 0 || (char)c == terminator) break;
		// grow in chunks rather than per byte when unbounded
		if (max == 0 && (count & 63) == 0) str.reserve(count + 64);
		size_t before = str.length();
		str += (char)c;
		if (str.length() == before) {	// append failed: out of memory
			setReadError();
			break;
		}
		count++;
	}
	return str;
}
