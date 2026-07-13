// RT1176 12-bit DAC (DAC12) — true analog voltage output on TP18 (EVKB).
// From-SDK bring-up (fsl_dac12 + RM ch.89): the RT1062/Teensy 4 has no DAC,
// so the API shape follows Teensy 3.x analogWriteDAC0().
//
// RM 89.1 chip-specifics: VREFH is the only reference (DACRFS must be 1),
// no HW trigger, PTAT current ref unavailable -> internal (CR2.IREF).
// A current reference MUST be selected or the DAC silently doesn't convert.
// Non-FIFO mode: every DATA write converts immediately (no trigger).

#include "imxrt1176.h"
#include "core_pins.h"

static uint8_t dac_inited = 0;

static void dac_init(void)
{
	CCM_LPCG57_DIRECT = 1u;                             // ungate DAC clock
	DAC_CR  = DAC_CR_DACRFS_MASK | DAC_CR_TRGSEL_MASK;  // VREFH ref, SW trigger, FIFO off
	DAC_CR2 = DAC_CR2_IREF_MASK | DAC_CR2_OEN_MASK;     // internal current ref, buffer bypassed
	DAC_CR |= DAC_CR_DACEN_MASK;
	dac_inited = 1;
}

void analogWriteDAC0(uint32_t value)
{
	if (!dac_inited) dac_init();
	uint32_t bits = analogWriteResBits();   // Teensy 3.x semantics: scale the
	if (bits < 12u) {                       // current analogWriteResolution to
		value <<= (12u - bits);             // the DAC's native 12 bits
	} else if (bits > 12u) {
		value >>= (bits - 12u);
	}
	if (value > DAC_DATA_DATA0_MASK) value = DAC_DATA_DATA0_MASK;
	DAC_DATA = value;                       // non-FIFO: converts immediately
}
