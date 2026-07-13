#include "imxrt1176.h"
#include "core_pins.h"   // analogWrite/analogWriteRes/analogWriteFrequency, pinMode, digitalWrite, OUTPUT

/* FlexPWM analogWrite engine.
 * A FlexPWM module has 4 submodules (SM0..3), each with output channels A/B and
 * an auxiliary X.  A write is: CLDOK (clear load-ok) -> program VAL/CTRL ->
 * OUTEN (enable pin) -> LDOK (latch) -> RUN.  Registers are 16-bit.
 * Only the header pins that route to FlexPWM are supported; others fall back to
 * a digital HIGH/LOW at the mid-point (Arduino convention). */

// ---- register access (16-bit; SM stride 0x60) ----
#define PWM_SM(base, sm, off) (*(volatile uint16_t *)((base) + (uint32_t)(sm)*0x60u + (off)))
#define SM_CTRL   0x06u
#define SM_CTRL2  0x04u
#define SM_INIT   0x02u
#define SM_VAL0   0x0Au   /* X duty */
#define SM_VAL1   0x0Eu   /* modulo (period-1) */
#define SM_VAL2   0x0Cu   /* A on-edge (kept 0) */
#define SM_VAL3   0x16u   /* A duty (off-edge) */
#define SM_VAL4   0x18u   /* B on-edge (kept 0) */
#define SM_VAL5   0x1Eu   /* B duty (off-edge) */
#define PWM_OUTEN(base)  (*(volatile uint16_t *)((base) + 0x180u))
#define PWM_MCTRL(base)  (*(volatile uint16_t *)((base) + 0x188u))
#define PWM_FCTRL0(base) (*(volatile uint16_t *)((base) + 0x18Cu))
#define PWM_FSTS0(base)  (*(volatile uint16_t *)((base) + 0x18Eu))
#define PWM_FFILT0(base) (*(volatile uint16_t *)((base) + 0x190u))
#define SMCTRL_PRSC(n)  ((uint16_t)(((n) & 0xF) << 4))
#define SMCTRL_FULL     ((uint16_t)(1u << 10))
#define SMCTRL2_INDEP   ((uint16_t)(1u << 13))
#define MCTRL_LDOK(m)   ((uint16_t)(((m) & 0xF) << 0))
#define MCTRL_CLDOK(m)  ((uint16_t)(((m) & 0xF) << 4))
#define MCTRL_RUN(m)    ((uint16_t)(((m) & 0xF) << 8))
#define OUTEN_X(m)      ((uint16_t)(((m) & 0xF) << 0))
#define OUTEN_B(m)      ((uint16_t)(((m) & 0xF) << 4))
#define OUTEN_A(m)      ((uint16_t)(((m) & 0xF) << 8))

/* FlexPWM peripheral clock (IPG bus clock).  Nominal; the Saleae verification
 * calibrates the absolute frequency and this constant is trimmed to match.
 * (Duty is a ratio, so duty% is correct regardless of this value.) */
#define PWM_CLOCK_HZ 24000000u

enum { CH_A, CH_B, CH_X };
struct pwm_pin { uint8_t pin; uint32_t base; uint8_t sm; uint8_t chan; uint8_t alt;
                 volatile uint32_t *mux; volatile uint32_t *pad; };

static const struct pwm_pin pwm_pins[] = {
	{ 9, FLEXPWM1_BASE, 0, CH_B, 4,   &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_01, &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_01 },
	{ 3, FLEXPWM1_BASE, 2, CH_A, 4,   &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_04, &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_04 },
	{ 5, FLEXPWM1_BASE, 2, CH_B, 4,   &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_05, &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_05 },
	{ 4, FLEXPWM1_BASE, 0, CH_X, 0xB, &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_06, &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_06 },
	{ 8, FLEXPWM1_BASE, 1, CH_X, 0xB, &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_07, &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_07 },
	{ 7, FLEXPWM3_BASE, 0, CH_X, 0xB, &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_14, &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_14 },
};
#define NPWM (sizeof(pwm_pins) / sizeof(pwm_pins[0]))

static uint32_t pwm_res_bits = 8;       // analogWriteResolution bits; range 0..(1<<bits)
static uint32_t pwm_freq     = 1000;    // default Hz
static uint16_t sm_modulo[4 * 4];       // [module 0..3][submodule 0..3]; 0 = not configured

static int pwm_slot(uint32_t base) {
	if (base == FLEXPWM1_BASE) return 0;
	if (base == FLEXPWM2_BASE) return 1;
	if (base == FLEXPWM3_BASE) return 2;
	return 3;
}

static const struct pwm_pin *find_pwm(uint8_t pin) {
	for (unsigned i = 0; i < NPWM; i++)
		if (pwm_pins[i].pin == pin) return &pwm_pins[i];
	return 0;
}

// Program a submodule for pwm_freq; enables the module clock; returns modulo (VAL1+1).
static uint16_t configure_sm(uint32_t base, uint8_t sm) {
	if (base == FLEXPWM1_BASE) CCM_LPCG79_DIRECT = 1u;   // ungate module clock
	else if (base == FLEXPWM3_BASE) CCM_LPCG81_DIRECT = 1u;
	/* Fault init: on reset, latched faults hold every output disabled (pins stay
	 * low). FLVL=15 makes faults active-high (idle fault inputs aren't faults),
	 * then clear the latched fault status so outputs can drive. */
	PWM_FCTRL0(base) = 0xF000u;
	PWM_FSTS0(base)  = 0x000Fu;
	PWM_FFILT0(base) = 0u;
	uint8_t prsc = 0; uint32_t modulo = 0;
	for (prsc = 0; prsc < 8u; prsc++) {                  // pick prescale so modulo fits 16-bit
		modulo = PWM_CLOCK_HZ / ((1u << prsc) * pwm_freq);
		if (modulo <= 65535u) break;
	}
	if (modulo < 2u) modulo = 2u;
	if (modulo > 65535u) modulo = 65535u;
	uint8_t mask = (uint8_t)(1u << sm);
	PWM_MCTRL(base) |= MCTRL_CLDOK(mask);
	PWM_SM(base, sm, SM_CTRL2) = SMCTRL2_INDEP;          // independent outputs, IPG clock (CLK_SEL=0)
	PWM_SM(base, sm, SM_CTRL)  = SMCTRL_FULL | SMCTRL_PRSC(prsc);
	PWM_SM(base, sm, SM_INIT)  = 0;
	PWM_SM(base, sm, SM_VAL2)  = 0;                      /* A on-edge (explicit, don't rely on reset) */
	PWM_SM(base, sm, SM_VAL4)  = 0;                      /* B on-edge */
	PWM_SM(base, sm, SM_VAL1)  = (uint16_t)(modulo - 1u);
	PWM_MCTRL(base) |= MCTRL_LDOK(mask);
	PWM_MCTRL(base) |= MCTRL_RUN(mask);
	sm_modulo[pwm_slot(base) * 4 + sm] = (uint16_t)modulo;
	return (uint16_t)modulo;
}

uint32_t analogWriteRes(uint32_t bits) {
	uint32_t prev = pwm_res_bits;
	if (bits < 1u) bits = 1u;
	if (bits > 16u) bits = 16u;
	pwm_res_bits = bits;
	return prev;
}

uint32_t analogWriteResBits(void) { return pwm_res_bits; }

void analogWrite(uint8_t pin, int value) {
	uint32_t range = 1u << pwm_res_bits;                 // full-scale (value==range => 100%)
	if (value < 0) value = 0;
	if ((uint32_t)value > range) value = range;
	const struct pwm_pin *p = find_pwm(pin);
	if (!p) {                                            // non-PWM pin: digital fallback
		pinMode(pin, OUTPUT);
		digitalWrite(pin, ((uint32_t)value >= range / 2u) ? 1 : 0);
		return;
	}
	int slot = pwm_slot(p->base) * 4 + p->sm;
	uint16_t modulo = sm_modulo[slot] ? sm_modulo[slot] : configure_sm(p->base, p->sm);
	uint32_t duty = (uint32_t)value * modulo / range;    // 0..modulo counts
	*p->mux = p->alt;                                    // route pad to FlexPWM
	*p->pad = 0x0008u;                                   // DSE drive, push-pull
	uint8_t mask = (uint8_t)(1u << p->sm);
	PWM_MCTRL(p->base) |= MCTRL_CLDOK(mask);
	if (p->chan == CH_A) {
		PWM_SM(p->base, p->sm, SM_VAL3) = (uint16_t)duty;  PWM_OUTEN(p->base) |= OUTEN_A(mask);
	} else if (p->chan == CH_B) {
		PWM_SM(p->base, p->sm, SM_VAL5) = (uint16_t)duty;  PWM_OUTEN(p->base) |= OUTEN_B(mask);
	} else { /* CH_X: on-time from VAL0 (inverted sense) */
		PWM_SM(p->base, p->sm, SM_VAL0) = (uint16_t)(modulo - duty);  PWM_OUTEN(p->base) |= OUTEN_X(mask);
	}
	PWM_MCTRL(p->base) |= MCTRL_LDOK(mask);
}

void analogWriteFrequency(uint8_t pin, float frequency) {
	const struct pwm_pin *p = find_pwm(pin);
	if (!p) return;                                      // ignore non-PWM pins (don't touch global freq)
	if (!(frequency >= 1.0f)) frequency = 1.0f;          // NaN-safe clamp
	pwm_freq = (uint32_t)frequency;
	int slot = pwm_slot(p->base) * 4 + p->sm;
	uint16_t oldm = sm_modulo[slot];
	uint16_t v0 = PWM_SM(p->base, p->sm, SM_VAL0);       // capture live duty counts
	uint16_t v3 = PWM_SM(p->base, p->sm, SM_VAL3);
	uint16_t v5 = PWM_SM(p->base, p->sm, SM_VAL5);
	sm_modulo[slot] = 0;                                 // force reconfigure at the new freq
	uint16_t newm = configure_sm(p->base, p->sm);        // reprograms VAL1; leaves VAL0/3/5
	if (oldm) {                                          // preserve duty PERCENTAGE across the change
		uint8_t mask = (uint8_t)(1u << p->sm);
		PWM_MCTRL(p->base) |= MCTRL_CLDOK(mask);
		PWM_SM(p->base, p->sm, SM_VAL0) = (uint16_t)((uint32_t)v0 * newm / oldm);
		PWM_SM(p->base, p->sm, SM_VAL3) = (uint16_t)((uint32_t)v3 * newm / oldm);
		PWM_SM(p->base, p->sm, SM_VAL5) = (uint16_t)((uint32_t)v5 * newm / oldm);
		PWM_MCTRL(p->base) |= MCTRL_LDOK(mask);
	}
}
