/* Arduino digital GPIO for NXP MIMXRT1170-EVKB (RT1176, Cortex-M7) -- Phase 0.
 *
 * Direct GPIO access (no fast-GPIO aliasing yet).  Only the EVKB User LED D6
 * (Arduino pin 13 / LED_BUILTIN -> GPIO9_IO03, pad GPIO_AD_04) is wired up;
 * other table entries are stubs.
 *
 * GPIOn register layout (offsets from the port base in digital_pin_to_info):
 *   DR        +0x00   data register
 *   GDIR      +0x04   direction (1 = output)
 *   PSR       +0x08   pad status (read input)
 *   DR_SET    +0x84   atomic set
 *   DR_CLEAR  +0x88   atomic clear
 *   DR_TOGGLE +0x8C   atomic toggle
 *
 * IOMUXC is configured faithfully for real hardware (Task 13).  Note: QEMU
 * models GPIO9 independently of IOMUXC, so the LED toggles in QEMU regardless
 * of the mux config -- that is expected.
 */

#include <stdint.h>
#include "imxrt1176.h"
#include "pins_arduino.h"

/* INPUT/OUTPUT/HIGH/LOW come from core_pins.h; define fallbacks so this file
 * also builds standalone (e.g. against the gpio probe).  Values must match
 * core_pins.h: HIGH=1 LOW=0 INPUT=0 OUTPUT=1. */
#ifndef OUTPUT
#define INPUT           0
#define OUTPUT          1
#define INPUT_PULLUP    2
#define INPUT_PULLDOWN  3
#define OUTPUT_OPENDRAIN 4
#endif

/* GPIOn register accessors from a port base address. */
#define GPIO_DR(base)        (*(volatile uint32_t *)((base) + 0x00))
#define GPIO_GDIR(base)      (*(volatile uint32_t *)((base) + 0x04))
#define GPIO_PSR(base)       (*(volatile uint32_t *)((base) + 0x08))
#define GPIO_DR_SET(base)    (*(volatile uint32_t *)((base) + 0x84))
#define GPIO_DR_CLEAR(base)  (*(volatile uint32_t *)((base) + 0x88))

/* A sane default pad-control value for a general-purpose output (drive
 * enabled, medium drive strength).  Exact electrical tuning is deferred;
 * QEMU ignores this and any reasonable value works on hardware for an LED. */
#define PAD_CTL_DEFAULT  0x0008u   /* DSE field set; pull/keeper off */

const struct digital_pin_info_struct digital_pin_to_info[CORE_NUM_DIGITAL] = {
	[13] = {
		.gpio     = GPIO9_BASE,
		.bit      = 3,
		.mux_mode = IOMUXC_PAD_GPIO_AD_04_GPIO9_IO03_ALT,   /* ALT10 */
		.mux      = &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_04,
		.pad      = &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_04,
	},
	/* pins 0..12 left as stubs (gpio == 0) for Phase 0 */
};

void pinMode(uint8_t pin, uint8_t mode)
{
	if (pin >= CORE_NUM_DIGITAL) return;
	const struct digital_pin_info_struct *p = &digital_pin_to_info[pin];
	if (p->gpio == 0) return;   /* unassigned stub */

	/* Route the pad to GPIO: write the ALT value into the mux register. */
	if (p->mux) *p->mux = p->mux_mode;
	/* Apply a sane pad configuration. */
	if (p->pad) *p->pad = PAD_CTL_DEFAULT;

	if (mode == OUTPUT || mode == OUTPUT_OPENDRAIN) {
		GPIO_GDIR(p->gpio) |= (1u << p->bit);
	} else {
		GPIO_GDIR(p->gpio) &= ~(1u << p->bit);
	}
}

void digitalWrite(uint8_t pin, uint8_t val)
{
	if (pin >= CORE_NUM_DIGITAL) return;
	const struct digital_pin_info_struct *p = &digital_pin_to_info[pin];
	if (p->gpio == 0) return;

	if (val) {
		GPIO_DR_SET(p->gpio) = (1u << p->bit);
	} else {
		GPIO_DR_CLEAR(p->gpio) = (1u << p->bit);
	}
}

uint8_t digitalRead(uint8_t pin)
{
	if (pin >= CORE_NUM_DIGITAL) return 0;
	const struct digital_pin_info_struct *p = &digital_pin_to_info[pin];
	if (p->gpio == 0) return 0;

	return (uint8_t)((GPIO_PSR(p->gpio) >> p->bit) & 1u);
}
