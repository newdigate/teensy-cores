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

#ifndef WProgram_h
#define WProgram_h

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Phase-0 (imxrt1176) trim: the full Teensy 4 WProgram.h pulls in the AVR
// emulation, HardwareSerial, the whole usb_* stack, WString, IntervalTimer,
// CrashReport, etc. None of those headers/sources exist yet in the minimal
// imxrt1176 core, and the blink sketch needs none of them. We include only
// wiring.h (-> core_pins.h: pinMode/digitalWrite/delay/millis, pin defines)
// and pins_arduino.h (LED_BUILTIN). Restore the rest as later phases land
// (Serial, USB, String, ...).
#include "wiring.h"

#ifdef __cplusplus

uint16_t makeWord(uint16_t w);
uint16_t makeWord(byte h, byte l);

#define word(...) makeWord(__VA_ARGS__)

// Measure the duration of a pulse at a digital pin.  The state may be
// either HIGH or LOW, to measure a pulse remains either high or low.  Return
// is pulse duration in microseconds, or zero if no pulse was detected.
unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L);
static unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L) __attribute__((unused));
// Measure the duration of a pulse at a digital pin.  The state may be
// either HIGH or LOW, to measure a pulse remains either high or low.  Return
// is pulse duration in microseconds, or zero if no pulse was detected.
static unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout) {
	return pulseIn(pin, state, timeout);
}

// Creates a square wave output on a digital pin at a specific frequency.  If
// duration is specified (in milliseconds), the waveform will automatically
// stop.  Otherwise, it continues into noTone() or another tone() call uses a
// different pin.  Only 1 waveform at a time is supported.  This waveform works
// on any digitial pin, but uses interrupts to create the output.  For more
// efficient output, use PWM pins with analogWriteFrequency().
void tone(uint8_t pin, uint16_t frequency, uint32_t duration = 0);
// Ends a square wave output created by a prior called to tone().
void noTone(uint8_t pin);

// WMath prototypes

// Returns a pseudo-random number as a 32 bit signed integer.  See randomSeed()
// for info about pseudo-random properties.
int32_t random(void);
// Returns a pseudo-random number less "howbig".  For example, random(100)
// will return numbers beteen 0 to 99.  See randomSeed() for info about
// pseudo-random properties.
uint32_t random(uint32_t howbig);
// Returns a pseudo-random within a range.  For example, random(50, 100)
// will return numbers beteen 50 to 99.  See randomSeed() for info about
// pseudo-random properties.
int32_t random(int32_t howsmall, int32_t howbig);
// Initialize the pseudo-random number generator.  Pseudo-random numbers
// follow a sequence, but the sequence is so long that initializing at an
// unpredictable starting point gives effectively random numbers useful
// for most non-cryptographic uses.  The Entropy library is recommended
// to produce a truly random number for randomSeed().  You could just use
// Entropy for all random numbers, but the speed is much slower than
// generating pseudo-random numbers.
void randomSeed(uint32_t newseed);
void srandom(unsigned int newseed);

#include "pins_arduino.h"

// Character-stream base classes, so a normal Arduino sketch gets String,
// Print, and Stream via Arduino.h (parity with Teensyduino). Placed near the
// end to avoid include cycles with wiring.h/core_pins.h. Each header has its
// own include guard; dependency order is WString -> Printable -> Print ->
// Stream (Print.h already pulls in WString.h/Printable.h; Stream.h pulls in
// Print.h), so this ordering is defensive but not strictly required.
#include "WString.h"
#include "Printable.h"
#include "Print.h"
#include "Stream.h"

#endif // __cplusplus


// Fast memcpy
#ifdef __cplusplus
extern "C" {
extern void *memcpy (void *dst, const void *src, size_t count);
}
#else
extern void *memcpy (void *dst, const void *src, size_t count);
#endif


#endif // WProgram_h
