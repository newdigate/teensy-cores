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

#if defined(ARDUINO_NOSH)
/* ------------------------------------------------------------------------
 * Nosh dev board (RT1176, same chip as the MIMXRT1170-EVKB, no Arduino
 * headers).  Pin numbers 3..21 keep the EVKB meaning wherever the pad still
 * exists as a GPIO on Nosh, so LED_BUILTIN = 3 and the A0..A5 numbers carry
 * over; 0..2 (the EVKB's Serial2 pads, still LPUART2 here) are reassigned to
 * spare pads, and 22..28 add the Nosh-only board signals.  Pads that belong to
 * a peripheral (LPUART1/2, SAI1, SEMC, FlexSPI, uSDHC1/2, USB, DSI, SWD) are
 * deliberately NOT in the table.  Names: NOSH_PIN_*; the pad -> signal map is
 * nosh-dev-board/docs/evkb-vs-nosh-firmware-porting.md sections 4 and 11.
 * ------------------------------------------------------------------------ */
#define LED_BUILTIN       3    /* GPIO3_IO03, pad GPIO_AD_04 via ALT5: red USER_LED D10, high = on */
#define CORE_NUM_DIGITAL  29
#define NUM_DIGITAL_PINS  CORE_NUM_DIGITAL
#define NUM_ANALOG_INPUTS 6    /* same ADC1 pads as the EVKB table; A0..A4 are unconnected on Nosh, A5 is USB_OTG2_ID */

#define NOSH_PIN_USER_LED      3   /* GPIO_AD_04, out, high = on */
#define NOSH_PIN_USB2_OC       4   /* GPIO_AD_06, in: TPS2553 FAULT, active low, open drain */
#define NOSH_PIN_LCD_TE        5   /* GPIO_AD_05: panel tearing-effect, NOT wired (R67 DNP) */
#define NOSH_PIN_CTP_INT       6   /* GPIO_AD_00, in: GT911 touch interrupt */
#define NOSH_PIN_WL_DEV_WAKE   8   /* GPIO_AD_07, out -> M.2 pin 66 (via U11) */
#define NOSH_PIN_CTP_RST       9   /* GPIO_AD_01, out: GT911 reset */
#define NOSH_PIN_WIFI_WAKE    10   /* GPIO_AD_29, in <- M.2 SDIO_WAKE# (via U12) */
#define NOSH_PIN_LCD_BL       11   /* GPIO_AD_30, out: panel backlight enable (no FlexPWM alt) */
#define NOSH_PIN_WL_RST       12   /* GPIO_AD_31, out: M.2 W_DISABLE1# (27R, 10k pull-up) */
#define NOSH_PIN_BT_DEV_WAKE  13   /* GPIO_AD_28, out -> M.2 pin 42 (via U11) */
#define NOSH_PIN_I2C5_SDA     14   /* GPIO_LPSR_04: LPI2C5 = Wire2 (SGTL5000 0x0A, GT911) */
#define NOSH_PIN_I2C5_SCL     15   /* GPIO_LPSR_05 */
#define NOSH_PIN_USB2_ID      21   /* GPIO_AD_08, in: J2 ID, low = A-plug (VBUS already switched in hardware) */
#define NOSH_PIN_SD_CD        22   /* GPIO_AD_34, in: low = card present (10k pull-up) */
#define NOSH_PIN_SD_PWREN     23   /* GPIO_AD_35, out: low = card powered (pulled low at reset) */
#define NOSH_PIN_BT_RST       24   /* GPIO_AD_15, out: M.2 W_DISABLE2# */
#define NOSH_PIN_SDIO_RST     25   /* GPIO_AD_16, out: M.2 SDIO_RESET# (via U11) */
#define NOSH_PIN_BT_WAKE      26   /* GPIO_AD_27, in <- M.2 UART_WAKE# (via U12) */
#define NOSH_PIN_LCD_PWR      27   /* GPIO_DISP_B2_15, out: LCM_PWR_EN (10k pull-up: on at reset) */
#define NOSH_PIN_LCD_RST      28   /* GPIO_AD_02, out: drive low to reset; released through D9 to the 1.8 V pull-up */
/* Spare pads with no connection on the board: 0 (GPIO_AD_03), 1 (GPIO_AD_26),
 * 2 (GPIO_AD_32), 7 (GPIO_AD_14), 16..19 (GPIO_AD_10..13), 20 (GPIO_AD_09). */
#else
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
#endif /* ARDUINO_NOSH / MIMXRT1170_EVKB */

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
#if defined(ARDUINO_NOSH)
/* Nosh has one I2C bus, LPI2C5 = Wire2 (codec + touch); LPI2C1's pads are not
 * wired (AD_09 unconnected, AD_08 is the USB2 ID input).  The SPI numbers above
 * are kept only so sketches compile: no LPSPI bus reaches a Nosh connector. */
#define PIN_WIRE_SDA  (14)
#define PIN_WIRE_SCL  (15)
#else
#define PIN_WIRE_SDA  (20)
#define PIN_WIRE_SCL  (21)
#endif

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
