/* Stream.h - byte-stream base class with timeout/parse/find helpers for the
 * imxrt1176 core.
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

#ifndef Stream_h
#define Stream_h

#include <inttypes.h>
#include "Print.h"
#include "WString.h"

// Lookahead policy for parseInt/parseFloat: what to do with non-numeric
// characters ahead of the number.
enum LookaheadMode {SKIP_ALL, SKIP_NONE, SKIP_WHITESPACE};

class Stream : public Print
{
public:
	constexpr Stream() : _timeout(1000), read_error(0) {}
	virtual int available() = 0;
	virtual int read() = 0;
	virtual int peek() = 0;

	// timeout (milliseconds) used by all timed operations below
	void setTimeout(unsigned long timeout)		{ _timeout = timeout; }
	unsigned long getTimeout(void) const		{ return _timeout; }

	// consume the stream until the target is matched (true) or the
	// stream times out / is exhausted (false); a NUL byte on the
	// stream is treated as end-of-data
	bool find(const char *target)			{ return findUntil(target, (const char *)NULL); }
	bool find(const uint8_t *target)		{ return find((const char *)target); }
	bool find(const String &target)			{ return find(target.c_str()); }
	bool find(const char *target, size_t length)	{ return findUntil(target, length, (const char *)0, 0); }
	bool find(const uint8_t *target, size_t length)	{ return find((const char *)target, length); }
	bool find(const String &target, size_t length)	{ return find(target.c_str(), length); }
	bool find(char target)				{ return find(&target, 1); }

	// as find(), but also give up (false) if the terminator string
	// is matched first
	bool findUntil(const char *target, const char *terminator);
	bool findUntil(const uint8_t *target, const char *terminator)	{ return findUntil((const char *)target, terminator); }
	bool findUntil(const String &target, const char *terminator)	{ return findUntil(target.c_str(), terminator); }
	bool findUntil(const char *target, const String &terminator)	{ return findUntil(target, terminator.c_str()); }
	bool findUntil(const String &target, const String &terminator)	{ return findUntil(target.c_str(), terminator.c_str()); }
	bool findUntil(const char *target, size_t targetLen, const char *terminate, size_t termLen);
	bool findUntil(const uint8_t *target, size_t targetLen, const char *terminate, size_t termLen)	{ return findUntil((const char *)target, targetLen, terminate, termLen); }
	bool findUntil(const String &target, size_t targetLen, const char *terminate, size_t termLen)	{ return findUntil(target.c_str(), targetLen, terminate, termLen); }
	bool findUntil(const char *target, size_t targetLen, const String &terminate, size_t termLen)	{ return findUntil(target, targetLen, terminate.c_str(), termLen); }
	bool findUntil(const String &target, size_t targetLen, const String &terminate, size_t termLen)	{ return findUntil(target.c_str(), targetLen, terminate.c_str(), termLen); }

	// parse a number from the stream; 0 on timeout / nothing parseable.
	// 'ignore' is a character skipped inside the number (e.g. ',')
	long parseInt(LookaheadMode lookahead = SKIP_ALL, char ignore = '\x01');
	float parseFloat(LookaheadMode lookahead = SKIP_ALL, char ignore = '\x01');

	// read up to length bytes (no null terminator added); returns count
	size_t readBytes(char *buffer, size_t length);
	size_t readBytes(uint8_t *buffer, size_t length)	{ return readBytes((char *)buffer, length); }
	// read until terminator (consumed, not stored); buffer is always
	// null terminated; returns count excluding the null
	size_t readBytesUntil(char terminator, char *buffer, size_t length);
	size_t readBytesUntil(char terminator, uint8_t *buffer, size_t length)	{ return readBytesUntil(terminator, (char *)buffer, length); }

	// accumulate into a String until timeout / NUL byte / max chars
	// (max == 0 means unlimited); readStringUntil also stops at the
	// terminator (consumed, not appended)
	String readString(size_t max = 0);
	String readStringUntil(char terminator, size_t max = 0);

	// read-error flag: set when a timed read operation times out or a
	// readString append fails (out of memory)
	int getReadError() const			{ return read_error; }
	void clearReadError()				{ setReadError(0); }

protected:
	void setReadError(int err = 1)			{ read_error = err; }
	int timedRead();	// read() with timeout; -1 on timeout
	int timedPeek();	// peek() with timeout; -1 on timeout
	int peekNextDigit(LookaheadMode lookahead, bool detectDecimal);

	unsigned long _timeout;	// milliseconds; read directly by derived classes
private:
	char read_error;
};

#endif
