/* Arduino digital GPIO for NXP MIMXRT1170-EVKB (RT1176, Cortex-M7).
 *
 * Direct GPIO access (no fast-GPIO aliasing yet).  Full Arduino-header
 * digital pin table: D0-D15 -> pins 0-15, A0-A5 -> pins 16-21.  Header
 * numbering: core pin N = header DN.  The green User LED shares the D3
 * pad (GPIO3_IO03, pad GPIO_AD_04 via ALT5) and is LED_BUILTIN (pin 3).
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
 * models the GPIO blocks independently of IOMUXC, so pins toggle in QEMU
 * regardless of the mux config -- that is expected.
 *
 * The header pins are driven through the "normal" GPIO ports (GPIO2/GPIO3/
 * GPIO5/GPIO6) selected by ALT5 (GPIO_MUXn), NOT the fast aliases GPIO8/9/11/12
 * (ALT10).  Reason: on the CM7 core the fast aliases have NO interrupt line
 * (their combined IRQs go to the CM4), whereas the normal ports own the CM7
 * GPIOn_Combined IRQs -- so attachInterrupt() only works on the normal ports.
 * The bit position within a port is identical across the fast/normal aliases,
 * so only the base and mux value change here.  We rely on the reset-default
 * IOMUXC_GPR GPIO_MUXn routing (-> normal GPIOn); no GPR write is needed.
 */

#include <stdint.h>
#include "imxrt1176.h"
#include "pins_arduino.h"
#include "wiring.h"   /* micros() prototype (core_pins.h), for pulseIn() below */

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
	/*
	 * .gpio = normal GPIO port base (ALT5 / GPIO_MUXn), .mux_mode = 0x5.
	 * Port map: AD_01..31 -> GPIO3, AD_00 -> GPIO2, DISP_B2 -> GPIO5,
	 * LPSR -> GPIO6.  .bit is the port bit (same as the old fast-alias bit).
	 */
	[0]  = { .gpio=GPIO5_BASE, .bit=12, .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_11, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_11 },
	[1]  = { .gpio=GPIO5_BASE, .bit=11, .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_10, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_10 },
	[2]  = { .gpio=GPIO5_BASE, .bit=13, .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_12, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_12 },
	[3]  = { .gpio=GPIO3_BASE, .bit=3,  .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_04, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_04 },
	[4]  = { .gpio=GPIO3_BASE, .bit=5,  .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_06, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_06 },
	[5]  = { .gpio=GPIO3_BASE, .bit=4,  .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_05, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_05 },
	[6]  = { .gpio=GPIO2_BASE, .bit=31, .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_00, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_00 },
	[7]  = { .gpio=GPIO3_BASE, .bit=13, .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_14, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_14 },
	[8]  = { .gpio=GPIO3_BASE, .bit=6,  .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_07, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_07 },
	[9]  = { .gpio=GPIO3_BASE, .bit=0,  .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_01, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_01 },
	[10] = { .gpio=GPIO3_BASE, .bit=28, .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_29, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_29 },
	[11] = { .gpio=GPIO3_BASE, .bit=29, .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_30, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_30 },
	[12] = { .gpio=GPIO3_BASE, .bit=30, .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_31, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_31 },
	[13] = { .gpio=GPIO3_BASE, .bit=27, .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_28, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_28 },
	[14] = { .gpio=GPIO6_BASE, .bit=4,  .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_LPSR_04, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_LPSR_04 },
	[15] = { .gpio=GPIO6_BASE, .bit=5,  .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_LPSR_05, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_LPSR_05 },
	[16] = { .gpio=GPIO3_BASE, .bit=9,  .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_10, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_10 },
	[17] = { .gpio=GPIO3_BASE, .bit=10, .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_11, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_11 },
	[18] = { .gpio=GPIO3_BASE, .bit=11, .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_12, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_12 },
	[19] = { .gpio=GPIO3_BASE, .bit=12, .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_13, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_13 },
	[20] = { .gpio=GPIO3_BASE, .bit=8,  .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_09, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_09 },
	[21] = { .gpio=GPIO3_BASE, .bit=7,  .mux_mode=0x5u, .mux=&IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_08, .pad=&IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_08 },
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

/* pulseIn: teensy4 digital.c port, adapted to our pin table (digital_pin_to_info
 * entries carry a GPIO base + bit instead of teensy4's raw reg pointers; the
 * PSR read replaces teensy4's *(p->reg + 2)). Same control flow otherwise. */
uint32_t pulseIn_high(uint8_t pin, uint32_t timeout)
{
	const struct digital_pin_info_struct *p = digital_pin_to_info + pin;
	const uint32_t mask = 1u << p->bit;
	uint32_t usec_start, usec_stop;

	// wait for any previous pulse to end
	usec_start = micros();
	while (GPIO_PSR(p->gpio) & mask) {
		if (micros() - usec_start > timeout) return 0;
	}
	// wait for the pulse to start
	usec_start = micros();
	while (!(GPIO_PSR(p->gpio) & mask)) {
		if (micros() - usec_start > timeout) return 0;
	}
	usec_start = micros();
	// wait for the pulse to stop
	while (GPIO_PSR(p->gpio) & mask) {
		if (micros() - usec_start > timeout) return 0;
	}
	usec_stop = micros();
	return usec_stop - usec_start;
}

uint32_t pulseIn_low(uint8_t pin, uint32_t timeout)
{
	const struct digital_pin_info_struct *p = digital_pin_to_info + pin;
	const uint32_t mask = 1u << p->bit;
	uint32_t usec_start, usec_stop;

	// wait for any previous pulse to end
	usec_start = micros();
	while (!(GPIO_PSR(p->gpio) & mask)) {
		if (micros() - usec_start > timeout) return 0;
	}
	// wait for the pulse to start
	usec_start = micros();
	while (GPIO_PSR(p->gpio) & mask) {
		if (micros() - usec_start > timeout) return 0;
	}
	usec_start = micros();
	// wait for the pulse to stop
	while (!(GPIO_PSR(p->gpio) & mask)) {
		if (micros() - usec_start > timeout) return 0;
	}
	usec_stop = micros();
	return usec_stop - usec_start;
}

uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout)
{
	if (pin >= CORE_NUM_DIGITAL) return 0;
	if (state) return pulseIn_high(pin, timeout);
	return pulseIn_low(pin, timeout);
}
