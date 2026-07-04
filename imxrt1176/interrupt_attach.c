#include "core_pins.h"     // digital_pin_to_info, mode macros, attachInterruptVector, IRQ_* enum
#include "imxrt1176.h"     // GPIO*_BASE, NVIC_ENABLE_IRQ

/*
 * GPIO pin-change interrupts on the CM7 core.
 *
 * The pin table drives the header through the "normal" GPIO ports GPIO2/3/5/6
 * (via ALT5 / GPIO_MUXn); those are the only port views whose combined
 * interrupt lines reach the CM7 NVIC.  Each port raises two combined lines:
 * {pins 0-15, pins 16-31}.  We attach one shared handler to every line we
 * enable and, on any GPIO interrupt, scan all four ports for pending+enabled
 * bits -- cheap and instance-agnostic.
 */

#define GPIO_ICR1(base)     (*(volatile uint32_t *)((base) + 0x0Cu))
#define GPIO_ICR2(base)     (*(volatile uint32_t *)((base) + 0x10u))
#define GPIO_IMR(base)      (*(volatile uint32_t *)((base) + 0x14u))
#define GPIO_ISR(base)      (*(volatile uint32_t *)((base) + 0x18u))
#define GPIO_EDGE_SEL(base) (*(volatile uint32_t *)((base) + 0x1Cu))

static void (*volatile isr_callback[CORE_NUM_DIGITAL])(void);

// The CM7-interrupt-capable "normal" GPIO ports the pin table targets.
static const uint32_t irq_ports[] = { GPIO2_BASE, GPIO3_BASE, GPIO5_BASE, GPIO6_BASE };

// Service every pending+enabled interrupt bit on one GPIO port.
static void dispatch_port(uint32_t base) {
	uint32_t pending = GPIO_ISR(base) & GPIO_IMR(base);
	if (!pending) return;
	GPIO_ISR(base) = pending;                       // W1C before callbacks (don't lose a re-trigger)
	for (uint8_t pin = 0; pin < CORE_NUM_DIGITAL; pin++) {
		const struct digital_pin_info_struct *p = &digital_pin_to_info[pin];
		if (p->gpio == base && (pending & (1u << p->bit)) && isr_callback[pin]) {
			isr_callback[pin]();
		}
	}
}

// Shared handler for every enabled GPIO combined line: scan all four ports.
static void gpio_isr(void) {
	for (unsigned i = 0; i < sizeof(irq_ports) / sizeof(irq_ports[0]); i++) {
		dispatch_port(irq_ports[i]);
	}
}

// Map a normal-GPIO port + bit to its CM7 combined IRQ (low=0-15, high=16-31).
// Returns -1 for a port with no CM7 interrupt line (e.g. a fast alias).
static int irq_for(uint32_t base, uint8_t bit) {
	int lo, hi;
	if      (base == GPIO2_BASE) { lo = IRQ_GPIO2_0_15; hi = IRQ_GPIO2_16_31; }
	else if (base == GPIO3_BASE) { lo = IRQ_GPIO3_0_15; hi = IRQ_GPIO3_16_31; }
	else if (base == GPIO5_BASE) { lo = IRQ_GPIO5_0_15; hi = IRQ_GPIO5_16_31; }
	else if (base == GPIO6_BASE) { lo = IRQ_GPIO6_0_15; hi = IRQ_GPIO6_16_31; }
	else return -1;
	return (bit < 16) ? lo : hi;
}

void attachInterrupt(uint8_t pin, void (*fn)(void), int mode) {
	if (pin >= CORE_NUM_DIGITAL || fn == 0) return;
	const struct digital_pin_info_struct *p = &digital_pin_to_info[pin];
	if (p->gpio == 0) return;                        // unmapped pin
	uint32_t base = p->gpio; uint8_t bit = p->bit;
	int irq = irq_for(base, bit);
	if (irq < 0) return;                             // port has no CM7 interrupt line

	// Validate mode before touching HW.  Note LOW/HIGH are LEVEL-sensitive: the
	// GPIO ISR bit re-latches every cycle the level is held, so the callback
	// keeps firing until the pin changes -- expected, matches Teensy, not a bug.
	uint32_t icr = 0; int any_edge = 0;
	switch (mode) {
		case LOW:     icr = 0; break;
		case HIGH:    icr = 1; break;
		case RISING:  icr = 2; break;
		case FALLING: icr = 3; break;
		case CHANGE:  any_edge = 1; break;
		default:      return;                        // unknown mode: change nothing
	}

	GPIO_IMR(base) &= ~(1u << bit);                  // mask while reconfiguring (avoid a mid-update fire)
	isr_callback[pin] = fn;
	if (any_edge) {
		GPIO_EDGE_SEL(base) |= (1u << bit);          // any edge
	} else {
		GPIO_EDGE_SEL(base) &= ~(1u << bit);
		if (bit < 16) {
			GPIO_ICR1(base) = (GPIO_ICR1(base) & ~(3u << (2 * bit))) | (icr << (2 * bit));
		} else {
			uint8_t s = 2 * (bit - 16);
			GPIO_ICR2(base) = (GPIO_ICR2(base) & ~(3u << s)) | (icr << s);
		}
	}
	GPIO_ISR(base) = (1u << bit);                    // clear stale status
	attachInterruptVector((IRQ_NUMBER_t)irq, gpio_isr);
	NVIC_SET_PRIORITY((IRQ_NUMBER_t)irq, 128);       // below serial(64)/I2C(16): user pin ISRs don't starve them
	NVIC_ENABLE_IRQ((IRQ_NUMBER_t)irq);
	GPIO_IMR(base) |= (1u << bit);                   // unmask last
}

void detachInterrupt(uint8_t pin) {
	if (pin >= CORE_NUM_DIGITAL) return;
	const struct digital_pin_info_struct *p = &digital_pin_to_info[pin];
	if (p->gpio == 0) return;
	GPIO_IMR(p->gpio) &= ~(1u << p->bit);
	GPIO_EDGE_SEL(p->gpio) &= ~(1u << p->bit);
	isr_callback[pin] = 0;
}
