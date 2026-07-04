#include "core_pins.h"     // digital_pin_to_info, mode macros, attachInterruptVector, IRQ_* enum
#include "imxrt1176.h"     // GPIO*_BASE, NVIC_ENABLE_IRQ

#define GPIO_ICR1(base)     (*(volatile uint32_t *)((base) + 0x0Cu))
#define GPIO_ICR2(base)     (*(volatile uint32_t *)((base) + 0x10u))
#define GPIO_IMR(base)      (*(volatile uint32_t *)((base) + 0x14u))
#define GPIO_ISR(base)      (*(volatile uint32_t *)((base) + 0x18u))
#define GPIO_EDGE_SEL(base) (*(volatile uint32_t *)((base) + 0x1Cu))

static void (*isr_callback[CORE_NUM_DIGITAL])(void);

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

static void gpio_isr_7_11(void) {                   // IRQ 99: GPIO7..11 (header pins on 8/9/11)
	dispatch_port(GPIO8_BASE);
	dispatch_port(GPIO9_BASE);
	dispatch_port(GPIO11_BASE);
}
static void gpio_isr_12(void) { dispatch_port(GPIO12_BASE); }   // IRQ 61/62: GPIO12 (D14/D15)

static void enable_irq_for(uint32_t base) {
	if (base == GPIO12_BASE) {
		attachInterruptVector(IRQ_GPIO12_0_15, gpio_isr_12);
		attachInterruptVector(IRQ_GPIO12_16_31, gpio_isr_12);
		NVIC_ENABLE_IRQ(IRQ_GPIO12_0_15);
		NVIC_ENABLE_IRQ(IRQ_GPIO12_16_31);
	} else {
		attachInterruptVector(IRQ_GPIO7_8_9_10_11, gpio_isr_7_11);
		NVIC_ENABLE_IRQ(IRQ_GPIO7_8_9_10_11);
	}
}

void attachInterrupt(uint8_t pin, void (*fn)(void), int mode) {
	if (pin >= CORE_NUM_DIGITAL || fn == 0) return;
	const struct digital_pin_info_struct *p = &digital_pin_to_info[pin];
	if (p->gpio == 0) return;                        // unmapped pin
	uint32_t base = p->gpio; uint8_t bit = p->bit;
	isr_callback[pin] = fn;
	if (mode == CHANGE) {
		GPIO_EDGE_SEL(base) |= (1u << bit);          // any edge
	} else {
		GPIO_EDGE_SEL(base) &= ~(1u << bit);
		uint32_t icr;                                // 0=low,1=high,2=rising,3=falling
		switch (mode) {
			case LOW:     icr = 0; break;
			case HIGH:    icr = 1; break;
			case RISING:  icr = 2; break;
			case FALLING: icr = 3; break;
			default:      icr = 2; break;
		}
		if (bit < 16) {
			GPIO_ICR1(base) = (GPIO_ICR1(base) & ~(3u << (2 * bit))) | (icr << (2 * bit));
		} else {
			uint8_t s = 2 * (bit - 16);
			GPIO_ICR2(base) = (GPIO_ICR2(base) & ~(3u << s)) | (icr << s);
		}
	}
	GPIO_ISR(base) = (1u << bit);                    // clear stale status
	GPIO_IMR(base) |= (1u << bit);                   // unmask
	enable_irq_for(base);
}

void detachInterrupt(uint8_t pin) {
	if (pin >= CORE_NUM_DIGITAL) return;
	const struct digital_pin_info_struct *p = &digital_pin_to_info[pin];
	if (p->gpio == 0) return;
	GPIO_IMR(p->gpio) &= ~(1u << p->bit);
	GPIO_EDGE_SEL(p->gpio) &= ~(1u << p->bit);
	isr_callback[pin] = 0;
}
