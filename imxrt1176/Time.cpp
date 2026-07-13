/* Time.cpp - civil-time conversion (breakTime / makeTime) for the
 * imxrt1176 core.
 *
 * Clean-room MIT implementation: written from the documented API contract
 * (Unix-epoch <-> DateTimeFields conversion) using Howard Hinnant's
 * public-domain civil-from-days / days-from-civil algorithms; not derived
 * from the LGPL Arduino/Teensyduino Time.cpp.
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
#include "core_pins.h"

// Every representable time (0 .. 0xFFFFFFFF) is 1970-01-01 .. 2106-02-07, so
// all dates are on or after the Unix epoch and every quantity below is
// non-negative: the math is done entirely in uint32_t.
//
// Day <-> date conversion uses Howard Hinnant's public-domain algorithms
// ("chrono-Compatible Low-Level Date Algorithms": days_from_civil and
// civil_from_days).  They work on a proleptic Gregorian calendar shifted to
// begin years on March 1, grouped into 400-year eras of exactly 146097 days;
// 719468 is the day offset between 0000-03-01 and 1970-01-01.

#define SECS_PER_DAY   86400u
#define SECS_PER_HOUR   3600u
#define SECS_PER_MIN      60u
#define DAYS_PER_ERA  146097u   // days in one 400-year Gregorian era
#define EPOCH_SHIFT   719468u   // days from 0000-03-01 to 1970-01-01
#define EPOCH_WDAY         4u   // 1970-01-01 was a Thursday

// days_from_civil (Hinnant): calendar date -> days since 1970-01-01.
// y = full calendar year (>= 1970), m = 1-12, d = 1-31.
static uint32_t days_from_civil(uint32_t y, uint32_t m, uint32_t d)
{
	y -= (m <= 2) ? 1u : 0u;             // shift Jan/Feb to previous March-year
	uint32_t era = y / 400u;
	uint32_t yoe = y - era * 400u;                            // [0, 399]
	uint32_t mp  = m + ((m > 2u) ? (uint32_t)-3 : 9u);        // [0, 11], Mar=0
	uint32_t doy = (153u * mp + 2u) / 5u + d - 1u;            // [0, 365]
	uint32_t doe = yoe * 365u + yoe / 4u - yoe / 100u + doy;  // [0, 146096]
	return era * DAYS_PER_ERA + doe - EPOCH_SHIFT;
}

// civil_from_days (Hinnant): days since 1970-01-01 -> calendar date.
static void civil_from_days(uint32_t days, uint32_t *y, uint32_t *m, uint32_t *d)
{
	uint32_t z   = days + EPOCH_SHIFT;
	uint32_t era = z / DAYS_PER_ERA;
	uint32_t doe = z - era * DAYS_PER_ERA;                    // [0, 146096]
	uint32_t yoe = (doe - doe / 1460u + doe / 36524u - doe / 146096u) / 365u;
	uint32_t yy  = yoe + era * 400u;
	uint32_t doy = doe - (365u * yoe + yoe / 4u - yoe / 100u);// [0, 365]
	uint32_t mp  = (5u * doy + 2u) / 153u;                    // [0, 11], Mar=0
	*d = doy - (153u * mp + 2u) / 5u + 1u;                    // [1, 31]
	*m = mp + ((mp < 10u) ? 3u : (uint32_t)-9);               // [1, 12]
	*y = yy + ((*m <= 2u) ? 1u : 0u);
}

void breakTime(uint32_t time, DateTimeFields &tm)
{
	uint32_t days = time / SECS_PER_DAY;   // <= 49710, fits easily
	uint32_t secs = time - days * SECS_PER_DAY;

	tm.hour = (uint8_t)(secs / SECS_PER_HOUR);
	secs -= (uint32_t)tm.hour * SECS_PER_HOUR;
	tm.min = (uint8_t)(secs / SECS_PER_MIN);
	tm.sec = (uint8_t)(secs - (uint32_t)tm.min * SECS_PER_MIN);

	tm.wday = (uint8_t)((days + EPOCH_WDAY) % 7u);   // 0 = Sunday

	uint32_t y, m, d;
	civil_from_days(days, &y, &m, &d);
	tm.mday = (uint8_t)d;
	tm.mon  = (uint8_t)(m - 1u);      // 0-11
	tm.year = (uint8_t)(y - 1900u);   // 70-206
}

uint32_t makeTime(const DateTimeFields &tm)
{
	uint32_t days = days_from_civil((uint32_t)tm.year + 1900u,
	                                (uint32_t)tm.mon + 1u,
	                                (uint32_t)tm.mday);
	return days * SECS_PER_DAY
	     + (uint32_t)tm.hour * SECS_PER_HOUR
	     + (uint32_t)tm.min * SECS_PER_MIN
	     + (uint32_t)tm.sec;
}
