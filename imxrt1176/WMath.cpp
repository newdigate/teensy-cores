/* WMath.cpp - Arduino random() / makeWord() API.
 *
 * Clean-room MIT implementation for the imxrt1176 core: xorshift32
 * (Marsaglia, "Xorshift RNGs", Journal of Statistical Software 2003 - a
 * public-domain algorithm) behind the documented Arduino random API; not
 * derived from the LGPL Wiring/Arduino WMath.cpp.
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

#include <stdint.h>

/* xorshift32 (Marsaglia 2003). State must never be zero; randomSeed(0) is
 * ignored (same contract as Arduino: a zero seed leaves the sequence alone). */
static uint32_t rng_state = 0x11760001u;

void randomSeed(uint32_t newseed)
{
	if (newseed != 0) rng_state = newseed;
}

void srandom(unsigned int newseed)
{
	if (newseed != 0) rng_state = newseed;
}

int32_t random(void)
{
	uint32_t x = rng_state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	rng_state = x;
	/* Arduino contract: non-negative result. */
	return (int32_t)(x >> 1);
}

uint32_t random(uint32_t howbig)
{
	if (howbig == 0) return 0;
	return (uint32_t)random() % howbig;
}

int32_t random(int32_t howsmall, int32_t howbig)
{
	if (howsmall >= howbig) return howsmall;
	return (int32_t)random((uint32_t)(howbig - howsmall)) + howsmall;
}

uint16_t makeWord(uint16_t w)
{
	return w;
}

uint16_t makeWord(unsigned char h, unsigned char l)
{
	return (uint16_t)((h << 8) | l);
}
