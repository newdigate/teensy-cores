/* Arduino tone()/noTone() for the MIMXRT1176 core.
 * Single square-wave tone on any header pin, driven by a shared IntervalTimer
 * (one of PIT1's 4 channels).  Mirrors cores/teensy4/tone.cpp; the pin is
 * toggled with the GPIO atomic DR_TOGGLE register. */
#include "Arduino.h"        // pinMode, digital_pin_to_info, __disable_irq, CORE_NUM_DIGITAL
#include "IntervalTimer.h"

/* GPIO atomic registers from a port base (see digital.c / imxrt1176.h). */
#define TONE_DR_CLEAR(base)  (*(volatile uint32_t *)((base) + 0x88u))
#define TONE_DR_TOGGLE(base) (*(volatile uint32_t *)((base) + 0x8Cu))

static uint8_t  tone_pin = 255;          /* 255 = idle */
static uint32_t tone_base = 0;
static uint32_t tone_mask = 0;
static volatile uint32_t tone_toggle_count = 0;   /* 0 = infinite */
static IntervalTimer tone_timer;

static void tone_interrupt(void)
{
    if (tone_toggle_count == 0) {                  /* infinite: just toggle */
        TONE_DR_TOGGLE(tone_base) = tone_mask;
        return;
    }
    if (--tone_toggle_count) {                      /* still toggling */
        TONE_DR_TOGGLE(tone_base) = tone_mask;
    } else {                                        /* last transition: end low, stop */
        TONE_DR_CLEAR(tone_base) = tone_mask;
        tone_timer.end();
        tone_pin = 255;
    }
}

void tone(uint8_t pin, uint16_t frequency, uint32_t duration)
{
    if (pin >= CORE_NUM_DIGITAL) return;
    const struct digital_pin_info_struct *p = &digital_pin_to_info[pin];
    if (p->gpio == 0) return;                       /* unmapped pin */
    if (frequency < 1) frequency = 1;
    float usec = 500000.0f / (float)frequency;      /* half-period */

    uint32_t count;
    if (duration) {
        count = (uint32_t)(((uint64_t)frequency * duration) / 1000u) * 2u;
        if (count < 2u) count = 2u;                 /* at least one full cycle */
    } else {
        count = 0;                                  /* infinite */
    }

    __disable_irq();
    if (pin != tone_pin) {
        if (tone_pin < CORE_NUM_DIGITAL) {
            TONE_DR_CLEAR(tone_base) = tone_mask;   /* stop previous pin */
        }
        tone_pin  = pin;
        tone_base = p->gpio;
        tone_mask = 1u << p->bit;
        pinMode(pin, OUTPUT);
        TONE_DR_CLEAR(tone_base) = tone_mask;       /* start low */
    }
    tone_toggle_count = count;
    tone_timer.begin(tone_interrupt, usec);         /* (re)start; simple restart on change */
    __enable_irq();
}

void noTone(uint8_t pin)
{
    if (pin >= CORE_NUM_DIGITAL) return;
    __disable_irq();
    if (pin == tone_pin) {
        tone_timer.end();
        TONE_DR_CLEAR(tone_base) = tone_mask;
        tone_pin = 255;
    }
    __enable_irq();
}
