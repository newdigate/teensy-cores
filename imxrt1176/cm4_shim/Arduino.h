// Arduino-lite for the freestanding CM4 image world. This is NOT the CM7
// Arduino.h: it provides the minimal surface AudioStream + gate nodes need.
// Grow it deliberately; anything with hardware behind it must come from a
// shared-core library or a probe-backed sequence, never from CM7 headers.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>   // freestanding prototypes; impls from runtime_stubs.c

#define EVKB_CM4_WORLD 1

// --- interrupt control (CM4 = same ARMv7-M primitives) ---
static inline void __disable_irq(void) { __asm volatile ("cpsid i" ::: "memory"); }
static inline void __enable_irq(void)  { __asm volatile ("cpsie i" ::: "memory"); }

// --- NVIC, CM4 world: raw registers, static vector table (no RAM vectors) ---
typedef int IRQ_NUMBER_t;
#define NVIC_ISER_BASE 0xE000E100u
#define NVIC_ICER_BASE 0xE000E180u
#define NVIC_ISPR_BASE 0xE000E200u
#define NVIC_IPRI_BASE 0xE000E400u
static inline void NVIC_ENABLE_IRQ(int n)  { *(volatile uint32_t *)(NVIC_ISER_BASE + 4u*((uint32_t)n >> 5)) = 1u << (n & 31); }
static inline void NVIC_DISABLE_IRQ(int n) { *(volatile uint32_t *)(NVIC_ICER_BASE + 4u*((uint32_t)n >> 5)) = 1u << (n & 31); }
static inline void NVIC_SET_PENDING(int n) { *(volatile uint32_t *)(NVIC_ISPR_BASE + 4u*((uint32_t)n >> 5)) = 1u << (n & 31); }
static inline void NVIC_SET_PRIORITY(int n, int p) { *(volatile uint8_t *)(NVIC_IPRI_BASE + (uint32_t)n) = (uint8_t)p; }

// The CM4 vector table is static (startup_cm4.S). attachInterruptVector is a
// no-op here: the handler symbol must already be in the table. AudioStream's
// software_isr is aliased to the table slot by the gate's startup file.
typedef void (*voidFuncPtr)(void);
static inline void attachInterruptVector(IRQ_NUMBER_t irq, voidFuncPtr f) { (void)irq; (void)f; }

// --- DWT cycle counter (Cortex-M4 has DWT/CYCCNT; enable in gate init) ---
#define ARM_DWT_CYCCNT (*(volatile uint32_t *)0xE0001004u)

// IRQ_SOFTWARE: same repurposed-CAN1 slot 44 as the CM7 world (CAN1 unused by
// repo convention on BOTH cores — FlexCAN work uses CAN3). AudioStream.h's
// #ifndef guard picks this up.
#define IRQ_SOFTWARE ((IRQ_NUMBER_t)44)

// F_CPU_ACTUAL — declaration only, mirroring core_pins.h:2919: AudioStream.h's
// in-class inline processorUsage()/processorUsageMax() parse
// CYCLE_COUNTER_APPROX_PERCENT, which names it. Nothing here DEFINES it: an
// image that odr-uses the CPU-usage-percent API fails at link instead of
// silently computing with a fake CM4 clock. (See cm4_shim/README.md log.)
extern volatile uint32_t F_CPU_ACTUAL;
