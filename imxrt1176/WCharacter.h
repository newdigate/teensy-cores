/* WCharacter.h - Arduino character classification / conversion API.
 *
 * Clean-room MIT implementation for the imxrt1176 core: written from the
 * documented Arduino API surface (thin wrappers over <ctype.h>), not derived
 * from the LGPL Wiring/Arduino WCharacter.h.
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

#pragma once

#include <ctype.h>

/* ctype.h functions have undefined behaviour for arguments outside
 * unsigned char / EOF; the Arduino API takes plain (possibly signed) chars,
 * so every wrapper casts through unsigned char first. */

static inline bool isAlpha(int c)             { return isalpha((unsigned char)c) != 0; }
static inline bool isAlphaNumeric(int c)      { return isalnum((unsigned char)c) != 0; }
static inline bool isAscii(int c)             { return c >= 0 && c <= 127; }
static inline bool isControl(int c)           { return iscntrl((unsigned char)c) != 0; }
static inline bool isDigit(int c)             { return isdigit((unsigned char)c) != 0; }
static inline bool isGraph(int c)             { return isgraph((unsigned char)c) != 0; }
static inline bool isHexadecimalDigit(int c)  { return isxdigit((unsigned char)c) != 0; }
static inline bool isLowerCase(int c)         { return islower((unsigned char)c) != 0; }
static inline bool isPrintable(int c)         { return isprint((unsigned char)c) != 0; }
static inline bool isPunct(int c)             { return ispunct((unsigned char)c) != 0; }
static inline bool isSpace(int c)             { return isspace((unsigned char)c) != 0; }
static inline bool isUpperCase(int c)         { return isupper((unsigned char)c) != 0; }
static inline bool isWhitespace(int c)        { return c == ' ' || c == '\t'; }

static inline int toAscii(int c)              { return c & 0x7f; }
static inline int toLowerCase(int c)          { return tolower((unsigned char)c); }
static inline int toUpperCase(int c)          { return toupper((unsigned char)c); }
