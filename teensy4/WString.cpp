/* WString.cpp - implementation of the Arduino String class
 * for the imxrt1176 core.
 *
 * Clean-room MIT implementation: written from the documented Arduino String
 * API surface and this core's extracted interface contract, not derived
 * from the LGPL Arduino/Wiring/Teensyduino WString sources.
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

#include "WString.h"

#ifdef __IMXRT1176_HOST_TEST__
// Host self-test build: no Arduino.h. The avr_functions.h formatting
// routines exist on the ARM target (newlib + this core's nonstd.c) but not
// in macOS/glibc libc, so file-local stand-ins are macro-routed here; the
// static names keep a host harness that also compiles nonstd.c linkable.
#include <stdio.h>

static char *host_radix(unsigned long long val, char *buf, int radix, int neg)
{
	static const char digit_chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char tmp[66];
	char *p = tmp + sizeof(tmp) - 1;
	if (radix < 2 || radix > 36) { buf[0] = 0; return buf; }
	*p = 0;
	do {
		*--p = digit_chars[val % (unsigned)radix];
		val /= (unsigned)radix;
	} while (val);
	if (neg) *--p = '-';
	return strcpy(buf, p);
}
static char *host_ltoa(long v, char *b, int r)
{
	if (r == 10 && v < 0)
		return host_radix(0ULL - (unsigned long long)(long long)v, b, r, 1);
	return host_radix((unsigned long)v, b, r, 0);
}
static char *host_ultoa(unsigned long v, char *b, int r) { return host_radix(v, b, r, 0); }
static char *host_lltoa(long long v, char *b, int r)
{
	if (r == 10 && v < 0)
		return host_radix(0ULL - (unsigned long long)v, b, r, 1);
	return host_radix((unsigned long long)v, b, r, 0);
}
static char *host_ulltoa(unsigned long long v, char *b, int r) { return host_radix(v, b, r, 0); }
static char *host_dtostrf(float val, int width, unsigned int precision, char *buf)
{
	sprintf(buf, "%*.*f", width, (int)precision, (double)val);
	return buf;
}
#define ltoa	host_ltoa
#define ultoa	host_ultoa
#define lltoa	host_lltoa
#define ulltoa	host_ulltoa
#define dtostrf	host_dtostrf
#endif	// __IMXRT1176_HOST_TEST__

// The one guaranteed-valid empty C string handed out for unallocated Strings.
static const char string_empty[1] = "";

// worst-case integer text: 64 binary digits + sign + NUL
#define STRING_INT_BUFLEN 66
// worst-case float text from dtostrf: ~40 integer digits + '.' + decimals
#define STRING_FLOAT_BUFLEN 128
#define STRING_FLOAT_MAX_DIGITS 64

/*---------------------------------------------------------------------------
 * private helpers
 *---------------------------------------------------------------------------*/

void String::init(void)
{
	buffer = NULL;
	capacity = 0;
	len = 0;
}

void String::invalidate(void)
{
	if (buffer) free(buffer);
	init();
}

// (re)allocate to hold exactly maxStrLen chars + NUL; content preserved,
// buffer left a valid C string; 0 on failure with the String untouched
unsigned char String::changeBuffer(unsigned int maxStrLen)
{
	if (maxStrLen + 1 == 0) return 0;	// size overflow
	char *newbuf = (char *)realloc(buffer, maxStrLen + 1);
	if (!newbuf) return 0;
	if (!buffer) newbuf[0] = 0;
	buffer = newbuf;
	capacity = maxStrLen;
	return 1;
}

// amortized growth for appends: double the capacity (min 16), fall back to
// the exact requirement if the doubled request cannot be satisfied
unsigned char String::grow(unsigned int minCapacity)
{
	if (buffer && capacity >= minCapacity) return 1;
	unsigned int request = capacity * 2;
	if (request < capacity) request = minCapacity;	// doubling overflowed
	if (request < minCapacity) request = minCapacity;
	if (request < 16) request = 16;
	if (changeBuffer(request)) return 1;
	if (request != minCapacity && changeBuffer(minCapacity)) return 1;
	return 0;
}

// alias-safe append core: cstr may point into this String's own buffer
// (s += s); on allocation failure this is a no-op
String & String::appendChars(const char *cstr, unsigned int count)
{
	if (!cstr || count == 0) return *this;
	unsigned int newlen = len + count;
	if (newlen < len) return *this;		// length overflow
	if (newlen > capacity || !buffer) {
		unsigned char aliased = 0;
		unsigned int offset = 0;
		if (buffer && cstr >= buffer && cstr <= buffer + len) {
			aliased = 1;
			offset = (unsigned int)(cstr - buffer);
		}
		if (!grow(newlen)) return *this;
		if (aliased) cstr = buffer + offset;	// realloc may have moved it
	}
	memmove(buffer + len, cstr, count);
	len = newlen;
	buffer[len] = 0;
	return *this;
}

/*---------------------------------------------------------------------------
 * constructors / destructor
 *---------------------------------------------------------------------------*/

String::String(const char *cstr)
{
	init();
	if (cstr) copy(cstr, (unsigned int)strlen(cstr));
}

String::String(const __FlashStringHelper *pgmstr)
{
	init();
	const char *p = (const char *)pgmstr;
	if (p) copy(p, (unsigned int)strlen(p));
}

String::String(const String &str)
{
	init();
	if (str.buffer) copy(str.buffer, str.len);
}

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
String::String(String &&rval)
{
	init();
	move(rval);
}

String::String(StringSumHelper &&rval)
{
	init();
	move(rval);
}
#endif

String::String(char c)
{
	init();
	if (c) copy(&c, 1);
	else copy("", 0);
}

String::String(unsigned char c)
{
	init();
	char buf[STRING_INT_BUFLEN];
	ultoa((unsigned long)c, buf, 10);
	copy(buf, (unsigned int)strlen(buf));
}

String::String(int value, unsigned char base)
{
	init();
	char buf[STRING_INT_BUFLEN];
	ltoa((long)value, buf, base);
	copy(buf, (unsigned int)strlen(buf));
}

String::String(unsigned int value, unsigned char base)
{
	init();
	char buf[STRING_INT_BUFLEN];
	ultoa((unsigned long)value, buf, base);
	copy(buf, (unsigned int)strlen(buf));
}

String::String(long value, unsigned char base)
{
	init();
	char buf[STRING_INT_BUFLEN];
	ltoa(value, buf, base);
	copy(buf, (unsigned int)strlen(buf));
}

String::String(unsigned long value, unsigned char base)
{
	init();
	char buf[STRING_INT_BUFLEN];
	ultoa(value, buf, base);
	copy(buf, (unsigned int)strlen(buf));
}

String::String(long long value, unsigned char base)
{
	init();
	char buf[STRING_INT_BUFLEN];
	lltoa(value, buf, base);
	copy(buf, (unsigned int)strlen(buf));
}

String::String(unsigned long long value, unsigned char base)
{
	init();
	char buf[STRING_INT_BUFLEN];
	ulltoa(value, buf, base);
	copy(buf, (unsigned int)strlen(buf));
}

String::String(float num, unsigned char digits)
{
	init();
	char buf[STRING_FLOAT_BUFLEN];
	if (digits > STRING_FLOAT_MAX_DIGITS) digits = STRING_FLOAT_MAX_DIGITS;
	dtostrf(num, 1, digits, buf);
	copy(buf, (unsigned int)strlen(buf));
}

String::String(double num, unsigned char digits)
{
	init();
	char buf[STRING_FLOAT_BUFLEN];
	if (digits > STRING_FLOAT_MAX_DIGITS) digits = STRING_FLOAT_MAX_DIGITS;
	dtostrf((float)num, 1, digits, buf);
	copy(buf, (unsigned int)strlen(buf));
}

String::~String(void)
{
	if (buffer) free(buffer);
}

/*---------------------------------------------------------------------------
 * memory management
 *---------------------------------------------------------------------------*/

unsigned char String::reserve(unsigned int size)
{
	if (buffer && capacity >= size) return 1;
	return changeBuffer(size);
}

/*---------------------------------------------------------------------------
 * copy / move / assignment
 *---------------------------------------------------------------------------*/

String & String::copy(const char *cstr, unsigned int length)
{
	if (!cstr) length = 0;
	if (!reserve(length)) {
		invalidate();	// allocation failed: empty/falsy per contract
		return *this;
	}
	if (length > 0) memmove(buffer, cstr, length);	// cstr may alias buffer
	len = length;
	buffer[len] = 0;
	return *this;
}

String & String::copy(const __FlashStringHelper *s)
{
	const char *p = (const char *)s;
	if (!p) {
		invalidate();
		return *this;
	}
	return copy(p, (unsigned int)strlen(p));
}

void String::move(String &rhs)
{
	if (this == &rhs) return;
	if (buffer) free(buffer);
	buffer = rhs.buffer;
	capacity = rhs.capacity;
	len = rhs.len;
	rhs.init();
}

String & String::operator = (const String &rhs)
{
	if (this != &rhs) {
		if (rhs.buffer) copy(rhs.buffer, rhs.len);
		else invalidate();
	}
	return *this;
}

String & String::operator = (const char *cstr)
{
	if (cstr) copy(cstr, (unsigned int)strlen(cstr));
	else invalidate();
	return *this;
}

String & String::operator = (const __FlashStringHelper *pgmstr)
{
	return operator = ((const char *)pgmstr);
}

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
String & String::operator = (String &&rval)
{
	move(rval);
	return *this;
}

String & String::operator = (StringSumHelper &&rval)
{
	move(rval);
	return *this;
}
#endif

String & String::operator = (char c)
{
	if (c) copy(&c, 1);
	else copy("", 0);
	return *this;
}

/*---------------------------------------------------------------------------
 * append
 *---------------------------------------------------------------------------*/

String & String::append(const String &str)
{
	return appendChars(str.buffer, str.len);
}

String & String::append(const char *cstr)
{
	if (!cstr) return *this;
	return appendChars(cstr, (unsigned int)strlen(cstr));
}

String & String::append(const __FlashStringHelper *s)
{
	return append((const char *)s);
}

String & String::append(char c)
{
	if (!c) return *this;
	return appendChars(&c, 1);
}

String & String::append(unsigned char c)
{
	// contract quirk: appends the decimal NUMBER, not the character
	return append((int)c);
}

String & String::append(int num)
{
	char buf[STRING_INT_BUFLEN];
	ltoa((long)num, buf, 10);
	return appendChars(buf, (unsigned int)strlen(buf));
}

String & String::append(unsigned int num)
{
	char buf[STRING_INT_BUFLEN];
	ultoa((unsigned long)num, buf, 10);
	return appendChars(buf, (unsigned int)strlen(buf));
}

String & String::append(long num)
{
	char buf[STRING_INT_BUFLEN];
	ltoa(num, buf, 10);
	return appendChars(buf, (unsigned int)strlen(buf));
}

String & String::append(unsigned long num)
{
	char buf[STRING_INT_BUFLEN];
	ultoa(num, buf, 10);
	return appendChars(buf, (unsigned int)strlen(buf));
}

String & String::append(long long num)
{
	char buf[STRING_INT_BUFLEN];
	lltoa(num, buf, 10);
	return appendChars(buf, (unsigned int)strlen(buf));
}

String & String::append(unsigned long long num)
{
	char buf[STRING_INT_BUFLEN];
	ulltoa(num, buf, 10);
	return appendChars(buf, (unsigned int)strlen(buf));
}

String & String::append(float num)
{
	char buf[STRING_FLOAT_BUFLEN];
	dtostrf(num, 1, 2, buf);
	return appendChars(buf, (unsigned int)strlen(buf));
}

String & String::append(double num)
{
	return append((float)num);
}

/*---------------------------------------------------------------------------
 * concatenate (free operator+ set) — append rhs into the accumulator
 *---------------------------------------------------------------------------*/

StringSumHelper & operator + (const StringSumHelper &lhs, const String &rhs)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(rhs);
	return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, const char *cstr)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(cstr);
	return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, const __FlashStringHelper *pgmstr)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(pgmstr);
	return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, char c)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(c);
	return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, unsigned char c)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(c);
	return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, int num)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(num);
	return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, unsigned int num)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(num);
	return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, long num)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(num);
	return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, unsigned long num)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(num);
	return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, long long num)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(num);
	return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, unsigned long long num)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(num);
	return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, float num)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(num);
	return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, double num)
{
	StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
	a.append(num);
	return a;
}

/*---------------------------------------------------------------------------
 * comparison
 *---------------------------------------------------------------------------*/

int String::compareTo(const String &s) const
{
	return strcmp(c_str(), s.c_str());
}

unsigned char String::equals(const String &s) const
{
	if (len != s.len) return 0;
	if (len == 0) return 1;
	return memcmp(buffer, s.buffer, len) == 0 ? 1 : 0;
}

unsigned char String::equals(const char *cstr) const
{
	if (!cstr) return len == 0 ? 1 : 0;	// NULL compares equal to ""
	return strcmp(c_str(), cstr) == 0 ? 1 : 0;
}

unsigned char String::equalsIgnoreCase(const String &s) const
{
	if (len != s.len) return 0;
	for (unsigned int i = 0; i < len; i++) {
		if (tolower((unsigned char)buffer[i]) != tolower((unsigned char)s.buffer[i]))
			return 0;
	}
	return 1;
}

unsigned char String::startsWith(const String &prefix) const
{
	return startsWith(prefix, 0);
}

unsigned char String::startsWith(const String &prefix, unsigned int offset) const
{
	if (offset > len || prefix.len > len - offset) return 0;
	if (prefix.len == 0) return 1;
	return memcmp(c_str() + offset, prefix.buffer, prefix.len) == 0 ? 1 : 0;
}

unsigned char String::endsWith(const String &suffix) const
{
	if (suffix.len > len) return 0;
	if (suffix.len == 0) return 1;
	return memcmp(c_str() + len - suffix.len, suffix.buffer, suffix.len) == 0 ? 1 : 0;
}

/*---------------------------------------------------------------------------
 * character access
 *---------------------------------------------------------------------------*/

char String::charAt(unsigned int index) const
{
	if (index >= len) return 0;
	return buffer[index];
}

void String::setCharAt(unsigned int index, char c)
{
	if (index < len) buffer[index] = c;
}

char String::operator [] (unsigned int index) const
{
	if (index >= len) return 0;
	return buffer[index];
}

char & String::operator [] (unsigned int index)
{
	// out-of-range writes land in a static dummy, reset each call
	static char dummy_writable_char;
	if (index >= len || !buffer) {
		dummy_writable_char = 0;
		return dummy_writable_char;
	}
	return buffer[index];
}

void String::getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index) const
{
	if (!buf || bufsize == 0) return;
	if (index >= len) {
		buf[0] = 0;
		return;
	}
	unsigned int n = bufsize - 1;
	if (n > len - index) n = len - index;
	memcpy(buf, buffer + index, n);
	buf[n] = 0;
}

const char * String::c_str() const
{
	return buffer ? buffer : string_empty;
}

char * String::begin()
{
	if (!buffer) reserve(20);	// ensure a writable buffer exists
	if (!buffer) {
		static char oom_fallback;
		oom_fallback = 0;
		return &oom_fallback;
	}
	return buffer;
}

char * String::end()
{
	return begin() + len;
}

const char * String::begin() const
{
	return c_str();
}

const char * String::end() const
{
	return c_str() + len;
}

/*---------------------------------------------------------------------------
 * search
 *---------------------------------------------------------------------------*/

int String::indexOf(char ch) const
{
	return indexOf(ch, 0);
}

int String::indexOf(char ch, unsigned int fromIndex) const
{
	if (fromIndex >= len) return -1;
	const char *p = (const char *)memchr(buffer + fromIndex, ch, len - fromIndex);
	return p ? (int)(p - buffer) : -1;
}

int String::indexOf(const String &str) const
{
	return indexOf(str, 0);
}

int String::indexOf(const String &str, unsigned int fromIndex) const
{
	if (fromIndex >= len) return -1;
	const char *p = strstr(buffer + fromIndex, str.c_str());
	return p ? (int)(p - buffer) : -1;
}

int String::lastIndexOf(char ch) const
{
	return lastIndexOf(ch, len - 1);
}

int String::lastIndexOf(char ch, unsigned int fromIndex) const
{
	if (fromIndex >= len) return -1;
	for (int i = (int)fromIndex; i >= 0; i--) {
		if (buffer[i] == ch) return i;
	}
	return -1;
}

int String::lastIndexOf(const String &str) const
{
	return lastIndexOf(str, len);
}

int String::lastIndexOf(const String &str, unsigned int fromIndex) const
{
	if (str.len == 0 || str.len > len) return -1;
	if (fromIndex > len - str.len) fromIndex = len - str.len;
	for (int i = (int)fromIndex; i >= 0; i--) {
		if (memcmp(buffer + i, str.buffer, str.len) == 0) return i;
	}
	return -1;
}

String String::substring(unsigned int beginIndex) const
{
	return substring(beginIndex, len);
}

String String::substring(unsigned int beginIndex, unsigned int endIndex) const
{
	if (beginIndex > endIndex) {
		unsigned int tmp = beginIndex;
		beginIndex = endIndex;
		endIndex = tmp;
	}
	String out;
	if (beginIndex >= len) return out;
	if (endIndex > len) endIndex = len;
	out.copy(buffer + beginIndex, endIndex - beginIndex);
	return out;
}

/*---------------------------------------------------------------------------
 * modification
 *---------------------------------------------------------------------------*/

String & String::replace(char find, char replace)
{
	for (unsigned int i = 0; i < len; i++) {
		if (buffer[i] == find) buffer[i] = replace;
	}
	return *this;
}

String & String::replace(const String &find, const String &replace)
{
	if (len == 0 || find.len == 0 || find.len > len) return *this;
	// count non-overlapping occurrences
	unsigned int count = 0;
	const char *p = buffer;
	while ((p = strstr(p, find.buffer)) != NULL) {
		count++;
		p += find.len;
	}
	if (count == 0) return *this;
	// substitute into a fresh buffer (uniform grow/shrink, no aliasing);
	// on allocation failure or size overflow the String is left unchanged
	if (replace.len > find.len &&
	    count > (0xFFFFFFFFu - len) / (replace.len - find.len)) return *this;
	unsigned int newlen = len + count * replace.len - count * find.len;
	char *out = (char *)malloc(newlen + 1);
	if (!out) return *this;
	const char *src = buffer;
	char *dst = out;
	while ((p = strstr(src, find.buffer)) != NULL) {
		memcpy(dst, src, (size_t)(p - src));
		dst += p - src;
		memcpy(dst, replace.c_str(), replace.len);
		dst += replace.len;
		src = p + find.len;
	}
	strcpy(dst, src);	// tail + NUL
	free(buffer);
	buffer = out;
	capacity = newlen;
	len = newlen;
	return *this;
}

String & String::remove(unsigned int index)
{
	if (index < len) {
		len = index;
		buffer[len] = 0;
	}
	return *this;
}

String & String::remove(unsigned int index, unsigned int count)
{
	if (index >= len || count == 0) return *this;
	if (count > len - index) count = len - index;
	memmove(buffer + index, buffer + index + count, len - index - count);
	len -= count;
	buffer[len] = 0;
	return *this;
}

String & String::toLowerCase(void)
{
	for (unsigned int i = 0; i < len; i++) {
		buffer[i] = (char)tolower((unsigned char)buffer[i]);
	}
	return *this;
}

String & String::toUpperCase(void)
{
	for (unsigned int i = 0; i < len; i++) {
		buffer[i] = (char)toupper((unsigned char)buffer[i]);
	}
	return *this;
}

String & String::trim(void)
{
	if (!buffer || len == 0) return *this;
	char *first = buffer;
	char *last = buffer + len - 1;
	while (isspace((unsigned char)*first)) first++;
	while (last >= first && isspace((unsigned char)*last)) last--;
	len = (unsigned int)(last + 1 - first);
	if (first > buffer) memmove(buffer, first, len);
	buffer[len] = 0;
	return *this;
}

/*---------------------------------------------------------------------------
 * parsing / conversion
 *---------------------------------------------------------------------------*/

long String::toInt(void) const
{
	if (!buffer) return 0;
	return atol(buffer);
}

float String::toFloat(void) const
{
	if (!buffer) return 0.0f;
	return strtof(buffer, NULL);
}
