/* Simple compatibility headers for AVR code used with NXP IMXRT chips
 * Copyright (c) 2018 Paul Stoffregen <paul@pjrc.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _AVR_IO_H_
#define _AVR_IO_H_ 1

// Minimal AVR-compat shim. Arduino libraries include <avr/io.h> by convention
// (it's where AVR register/port definitions traditionally live). On RT1176 the
// AVR port-register emulation (avr_emulation.h) is NOT wired up here: it has a
// pre-existing core_pins.h/pins_arduino.h include-order issue and is currently
// unused (dead code), so this header intentionally stays minimal — enough for
// consumers like EEPROM.h that include <avr/io.h> defensively but use nothing
// from it. If a future library needs the AVR port emulation, wire up (and fix)
// avr_emulation.h then.
#include <stdint.h>

#endif
