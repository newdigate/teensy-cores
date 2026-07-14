/* WString.h - Arduino String class for the imxrt1176 core.
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

#ifndef String_class_h
#define String_class_h
#ifdef __cplusplus

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "avr_functions.h"	// dtostrf + the ltoa/ultoa integer formatters
#include "avr/pgmspace.h"	// not used by String itself; kept because
				// ecosystem libraries expect it transitively

// Flash-string tag type: never defined, used only as an opaque pointer.
// On this core flash and RAM share one address space, so a
// const __FlashStringHelper * is simply a disguised const char *.
class __FlashStringHelper;
#ifndef F
#define F(string_literal) ((const __FlashStringHelper *)(string_literal))
#endif

class StringSumHelper;

// Arduino String: heap-backed, NUL-terminated whenever allocated.
// Growth: reserve() allocates exactly the requested capacity; appends grow
// by doubling (minimum 16 chars) so repeated += is amortized O(1)/char.
// A String with no buffer is the "invalid" (falsy) state: length() == 0,
// c_str() returns a static "" and every operation remains safe.
class String
{
private:
	// safe-bool idiom: implicit conversion to a pointer-to-member type
	// lets if(s)/while(s)/!s work while blocking accidental arithmetic
	typedef void (String::*StringBoolType)() const;
	void StringBoolHelper() const {}
public:
	// constructors — a NULL cstr yields the invalid (falsy) empty String;
	// none is explicit (implicit conversion from literals/chars/numbers
	// is part of the API)
	String(const char *cstr = (const char *)NULL);
	String(const __FlashStringHelper *pgmstr);
	String(const String &str);
	#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
	String(String &&rval);
	String(StringSumHelper &&rval);
	#endif
	String(char c);
	String(unsigned char c);		// formats the number, base 10
	String(int, unsigned char base=10);
	String(unsigned int, unsigned char base=10);
	String(long, unsigned char base=10);
	String(unsigned long, unsigned char base=10);
	String(long long, unsigned char base=10);
	String(unsigned long long, unsigned char base=10);
	String(float num, unsigned char digits=2);
	String(double num, unsigned char digits=2);	// float precision
	~String(void);

	// memory management — reserve returns 1 on success, 0 on alloc failure
	// (content is preserved either way)
	unsigned char reserve(unsigned int size);
	unsigned int length(void) const			{ return len; }

	// copy and move
	String & copy(const char *cstr, unsigned int length);
	String & copy(const __FlashStringHelper *s);
	void move(String &rhs);		// steal rhs's buffer; rhs left empty/falsy
	String & operator = (const String &rhs);
	String & operator = (const char *cstr);
	String & operator = (const __FlashStringHelper *pgmstr);
	#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
	String & operator = (String &&rval);
	String & operator = (StringSumHelper &&rval);
	#endif
	String & operator = (char c);

	// append — on allocation failure the append is a no-op returning *this;
	// appending a String to itself (s += s) is handled correctly
	String & append(const String &str);
	String & append(const char *cstr);
	String & append(const __FlashStringHelper *s);
	String & append(char c);
	String & append(unsigned char c);	// appends the NUMBER, not the character
	String & append(int num);
	String & append(unsigned int num);
	String & append(long num);
	String & append(unsigned long num);
	String & append(long long num);
	String & append(unsigned long long num);
	String & append(float num);		// 2 decimal places
	String & append(double num);		// float precision
	String & operator += (const String &rhs)	{ return append(rhs); }
	String & operator += (const char *cstr)		{ return append(cstr); }
	String & operator += (const __FlashStringHelper *pgmstr) { return append(pgmstr); }
	String & operator += (char c)			{ return append(c); }
	String & operator += (unsigned char c)		{ return append(c); }
	String & operator += (int num)			{ return append(num); }
	String & operator += (unsigned int num)		{ return append(num); }
	String & operator += (long num)			{ return append(num); }
	String & operator += (unsigned long num)	{ return append(num); }
	String & operator += (long long num)		{ return append(num); }
	String & operator += (unsigned long long num)	{ return append(num); }
	String & operator += (float num)		{ return append(num); }
	String & operator += (double num)		{ return append(num); }

	// concatenate — each operator+ appends its right operand into the
	// (writable) StringSumHelper accumulator and returns it, so chains
	// like String("a") + x + 42 build in one buffer
	friend StringSumHelper & operator + (const StringSumHelper &lhs, const String &rhs);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, const char *cstr);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, const __FlashStringHelper *pgmstr);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, char c);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned char c);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, int num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned int num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, long num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned long num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, long long num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned long long num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, float num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, double num);
	String & concat(const String &str)		{ return append(str); }
	String & concat(const char *cstr)		{ return append(cstr); }
	String & concat(const __FlashStringHelper *pgmstr) { return append(pgmstr); }
	String & concat(char c)				{ return append(c); }
	String & concat(unsigned char c)		{ return append(c); }
	String & concat(int num)			{ return append(num); }
	String & concat(unsigned int num)		{ return append(num); }
	String & concat(long num)			{ return append(num); }
	String & concat(unsigned long num)		{ return append(num); }
	String & concat(long long num)			{ return append(num); }
	String & concat(unsigned long long num)		{ return append(num); }
	String & concat(float num)			{ return append(num); }
	String & concat(double num)			{ return append(num); }

	// comparison — compareTo has strcmp semantics; the rest return
	// unsigned char 0/1
	int compareTo(const String &s) const;
	unsigned char equals(const String &s) const;
	unsigned char equals(const char *cstr) const;	// NULL compares equal to ""
	unsigned char operator == (const String &rhs) const	{ return equals(rhs); }
	unsigned char operator == (const char *cstr) const	{ return equals(cstr); }
	unsigned char operator != (const String &rhs) const	{ return !equals(rhs); }
	unsigned char operator != (const char *cstr) const	{ return !equals(cstr); }
	unsigned char operator <  (const String &rhs) const	{ return compareTo(rhs) < 0; }
	unsigned char operator >  (const String &rhs) const	{ return compareTo(rhs) > 0; }
	unsigned char operator <= (const String &rhs) const	{ return compareTo(rhs) <= 0; }
	unsigned char operator >= (const String &rhs) const	{ return compareTo(rhs) >= 0; }
	unsigned char equalsIgnoreCase(const String &s) const;
	unsigned char startsWith(const String &prefix) const;
	unsigned char startsWith(const String &prefix, unsigned int offset) const;
	unsigned char endsWith(const String &suffix) const;

	// character access — out-of-range reads return 0, out-of-range writes
	// land in a harmless static dummy; setCharAt out of range is a no-op
	char charAt(unsigned int index) const;
	void setCharAt(unsigned int index, char c);
	char operator [] (unsigned int index) const;
	char& operator [] (unsigned int index);
	// getBytes: copy up to bufsize-1 chars starting at index, ALWAYS
	// NUL-terminate (bufsize 0 or NULL buf = no-op); index >= length()
	// stores just a NUL — Print::print(const String &) iterates the
	// string through successive windows of this exact semantic
	void getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index=0) const;
	void toCharArray(char *buf, unsigned int bufsize, unsigned int index=0) const
		{ getBytes((unsigned char *)buf, bufsize, index); }
	const char * c_str() const;	// NEVER NULL: static "" when unallocated
	char * begin();			// allocates a small buffer if none exists
	char * end();
	const char * begin() const;
	const char * end() const;

	// search — -1 when not found or fromIndex out of range
	int indexOf( char ch ) const;
	int indexOf( char ch, unsigned int fromIndex ) const;
	int indexOf( const String &str ) const;
	int indexOf( const String &str, unsigned int fromIndex ) const;
	int lastIndexOf( char ch ) const;
	int lastIndexOf( char ch, unsigned int fromIndex ) const;
	int lastIndexOf( const String &str ) const;
	int lastIndexOf( const String &str, unsigned int fromIndex ) const;
	// substring: swapped args are switched, out-of-range clamps
	String substring( unsigned int beginIndex ) const;
	String substring( unsigned int beginIndex, unsigned int endIndex ) const;

	// modification — all return *this so calls chain
	String & replace(char find, char replace);
	String & replace(const String& find, const String& replace);
	String & remove(unsigned int index);
	String & remove(unsigned int index, unsigned int count);
	String & toLowerCase(void);
	String & toUpperCase(void);
	String & trim(void);

	// parsing/conversion — atol/strtof prefix semantics, garbage yields 0
	long toInt(void) const;
	float toFloat(void) const;

	// safe-bool: true once a buffer exists, false when unallocated
	// (default-empty, moved-from, or after allocation failure)
	operator StringBoolType() const
		{ return buffer ? &String::StringBoolHelper : (StringBoolType)0; }

private:
	char *buffer;		// NUL-terminated storage; NULL when unallocated
	unsigned int capacity;	// max chars (excluding NUL) buffer can hold
	unsigned int len;	// current chars (excluding NUL)

	void init(void);
	void invalidate(void);
	unsigned char changeBuffer(unsigned int maxStrLen);	// exact (re)alloc
	unsigned char grow(unsigned int minCapacity);		// amortized doubling
	String & appendChars(const char *cstr, unsigned int count); // alias-safe core
};

class StringSumHelper : public String
{
public:
	StringSumHelper(const String &s) : String(s) {}
	StringSumHelper(const char *p) : String(p) {}
	StringSumHelper(const __FlashStringHelper *pgmstr) : String(pgmstr) {}
	StringSumHelper(char c) : String(c) {}
	StringSumHelper(unsigned char c) : String(c) {}
	StringSumHelper(int num) : String(num) {}
	StringSumHelper(unsigned int num) : String(num) {}
	StringSumHelper(long num) : String(num) {}
	StringSumHelper(unsigned long num) : String(num) {}
	StringSumHelper(long long num) : String(num) {}
	StringSumHelper(unsigned long long num) : String(num) {}
};

#endif	// __cplusplus
#endif	// String_class_h
