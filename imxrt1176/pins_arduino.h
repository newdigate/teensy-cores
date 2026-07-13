/* Teensyduino Core Library -- NXP MIMXRT1170-EVKB (RT1176, Cortex-M7) port
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2018 PJRC.COM, LLC.
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

#ifndef pins_macros_for_arduino_compatibility_h
#define pins_macros_for_arduino_compatibility_h

#include <stdint.h>
#include "imxrt1176.h"

/* ------------------------------------------------------------------------
 * MIMXRT1170-EVKB Arduino-header digital pin table.
 *
 * Header numbering: core pin N = header DN (D0-D15 -> pins 0-15,
 * A0-A5 -> pins 16-21).  LED_BUILTIN is pin 3 (green User LED shares the
 * D3 pad, GPIO_AD_04).
 *
 * The table maps an Arduino pin number to the registers/fields the GPIO
 * driver (digital.c) needs:
 *   gpio  : GPIOn base address (DR @ +0x00, GDIR @ +0x04, PSR @ +0x08,
 *           DR_SET @ +0x84, DR_CLEAR @ +0x88, DR_TOGGLE @ +0x8C)
 *   bit   : bit index within that GPIO port
 *   mux   : IOMUXC SW_MUX_CTL_PAD register for the pad
 *   pad   : IOMUXC SW_PAD_CTL_PAD register for the pad
 *   mux_mode : ALT value written to the mux register to route pad -> GPIO
 * ------------------------------------------------------------------------ */

#define LED_BUILTIN       3    /* GPIO3_IO03, pad GPIO_AD_04 via ALT5 (green User LED, D3) */
#define CORE_NUM_DIGITAL  22
#define NUM_DIGITAL_PINS  CORE_NUM_DIGITAL
#define NUM_ANALOG_INPUTS 6

/* Arduino analog header J26 (RevC3 schematic sheet 26), even/socket pins.
 * ADC channel/side are the pads' dedicated ADC1 inputs (RM, no muxing):
 *   A0 = GPIO_AD_10 = ADC1 CH2A     A3 = GPIO_AD_13 = ADC1 CH3B
 *   A1 = GPIO_AD_11 = ADC1 CH2B     A4 = GPIO_AD_09 = ADC1 CH1B (LPI2C1 SDA)
 *   A2 = GPIO_AD_12 = ADC1 CH3A     A5 = GPIO_AD_08 = ADC1 CH1A (LPI2C1 SCL) */
#define A0 16
#define A1 17
#define A2 18
#define A3 19
#define A4 20
#define A5 21

typedef struct analog_pin_info_struct {
    uint8_t pin;       /* Arduino pin id */
    uint8_t instance;  /* 0 = LPADC1, 1 = LPADC2 */
    uint8_t channel;   /* CMDL.ADCH channel */
    uint8_t side;      /* 0 = A, 1 = B */
} analog_pin_info_t;

extern const analog_pin_info_t analog_pin_to_info[NUM_ANALOG_INPUTS];

#define NOT_AN_INTERRUPT  -1
#define NOT_ON_TIMER      0

/* IOMUXC mux ALT value routing an AD pad to the normal GPIO3 (GPIO_MUX3).
 * From SDK fsl_iomuxc.h: IOMUXC_GPIO_AD_04_GPIO_MUX3_IO03 => muxMode 0x5 (ALT5).
 * The pin table uses ALT5 (normal GPIO2/3/5/6), not ALT10 (fast GPIO8/9/11/12),
 * because only the normal ports have a CM7 interrupt line -- see digital.c and
 * interrupt_attach.c.  (ALT10 -> GPIO9 has no CM7 IRQ; attachInterrupt needs the
 * normal port.) */
#define IOMUXC_PAD_GPIO_AD_04_GPIO_MUX3_IO03_ALT  0x5

struct digital_pin_info_struct {
	uint32_t gpio;      /* GPIOn base address, 0 = unassigned stub */
	uint8_t  bit;       /* bit index within the GPIO port */
	uint8_t  mux_mode;  /* IOMUXC mux ALT value to route pad -> GPIO */
	volatile uint32_t *mux;  /* IOMUXC SW_MUX_CTL_PAD register */
	volatile uint32_t *pad;  /* IOMUXC SW_PAD_CTL_PAD register */
};

extern const struct digital_pin_info_struct digital_pin_to_info[CORE_NUM_DIGITAL];

/* ---- legacy Teensy macros some sketches reference (harmless stubs) ------- */
#define digitalPinToInterrupt(p)  ((p) < NUM_DIGITAL_PINS ? (p) : -1)
#define digitalPinToPCICR(p)    ((volatile uint8_t *)0)
#define digitalPinToPCICRbit(p) (0)
#define digitalPinToPCIFR(p)    ((volatile uint8_t *)0)
#define digitalPinToPCIFRbit(p) (0)
#define digitalPinToPCMSK(p)    ((volatile uint8_t *)0)
#define digitalPinToPCMSKbit(p) (0)

#define PIN_SPI_SS    (10)
#define PIN_SPI_MOSI  (11)
#define PIN_SPI_MISO  (12)
#define PIN_SPI_SCK   (13)
static const uint8_t SS = 10;
static const uint8_t MOSI = 11;
static const uint8_t MISO = 12;
static const uint8_t SCK = 13;

/* Wire (LPI2C1) rides the classic-Uno A4/A5 positions = our pins 20/21.
 * (The stock teensy4 values 18/19 land on A2/A3 in THIS numbering; the socket
 * positions silkscreened SDA/SCL next to AREF are D14/D15 = LPI2C5 = Wire2,
 * shared with the WM8962 codec.) */
#define PIN_WIRE_SDA  (20)
#define PIN_WIRE_SCL  (21)

#define PIN_SERIAL_RX (0)
#define PIN_SERIAL_TX (1)

static inline uint8_t digitalPinToTimer(uint8_t) __attribute__((always_inline, unused));
static inline uint8_t digitalPinToTimer(uint8_t pin __attribute__((unused)))
{
	return NOT_ON_TIMER;
}

// SerialUSB: NOT a macro alias here (unlike stock teensy4).  usb_serial.h
// (Phase 2) declares a real `extern usb_serial_class &SerialUSB;` reference
// object instead -- a textual #define SerialUSB->Serial would rewrite that
// declaration's own name and fail to compile.

#endif // pins_macros_for_arduino_compatibility_h
