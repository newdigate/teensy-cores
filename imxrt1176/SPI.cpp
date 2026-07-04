#include "SPI.h"

// CR
#define CR_MEN   (1u<<0)
#define CR_RST   (1u<<1)
// CFGR1
#define CFGR1_MASTER (1u<<0)
// TCR fields
#define TCR_FRAMESZ(n)  ((uint32_t)((n) & 0xFFFu))   // n = bits-1
#define TCR_PRESCALE(p) (((uint32_t)(p) & 0x7u) << 27)
#define TCR_CPHA (1u<<30)
#define TCR_CPOL (1u<<31)
#define TCR_LSBF (1u<<23)
// RSR
#define RSR_RXEMPTY (1u<<1)

#define SPI_TIMEOUT 100000u

void SPIClass::begin() {
	hw->lpcg = 1u;                              // ungate LPSPI clock
	hw->clock_root = hw->clock_root_val;        // mux 0 => 24 MHz
	hw->sck_mux = hw->sck_mux_val;  hw->sck_pad = hw->pad_ctl_val;
	hw->sdo_mux = hw->sdo_mux_val;  hw->sdo_pad = hw->pad_ctl_val;
	hw->sdi_mux = hw->sdi_mux_val;  hw->sdi_pad = hw->pad_ctl_val;
	hw->sck_select_input = hw->sck_select_val;
	hw->sdo_select_input = hw->sdo_select_val;
	hw->sdi_select_input = hw->sdi_select_val;
	hw->cr = CR_RST;  hw->cr = 0u;              // reset the block (MEN=0)
	hw->cfgr1 = CFGR1_MASTER;                   // master mode (write while MEN=0)
	tcr_base = 0u;                              // MODE0, MSB first (prescale added next)
	setClockDivider(4000000);                   // default 4 MHz: writes CCR, ORs prescale into tcr_base
	hw->cr = CR_MEN;                            // enable
}

void SPIClass::end() { hw->cr = 0u; hw->lpcg = 0u; }

// Program CCR.SCKDIV and the PRESCALE bits of tcr_base for the requested SCK.
// SCK = func_clock / (prescale_div * (SCKDIV + 2)); pick smallest prescale with
// SCKDIV in [0,255] giving SCK <= clockHz.
void SPIClass::setClockDivider(uint32_t clockHz) {
	if (clockHz == 0u) clockHz = 1000u;          // guard divide-by-zero; clamp to slow
	uint32_t prescale = 0, sckdiv = 0;
	for (prescale = 0; prescale < 8u; prescale++) {
		uint32_t pdiv = 1u << prescale;
		uint32_t denom = pdiv * clockHz;
		uint32_t div = (func_clock + denom - 1u) / denom;   // ceil(func/(pdiv*clk))
		if (div < 2u) div = 2u;
		sckdiv = div - 2u;
		if (sckdiv <= 255u) break;
	}
	if (prescale > 7u) { prescale = 7u; sckdiv = 255u; }
	uint32_t men = hw->cr & CR_MEN;
	hw->cr = 0u;                                 // CCR is writable only with MEN=0
	hw->ccr = (hw->ccr & ~0xFFu) | (sckdiv & 0xFFu);
	if (men) hw->cr = CR_MEN;
	tcr_base = (tcr_base & ~(0x7u << 27)) | TCR_PRESCALE(prescale);
}

void SPIClass::beginTransaction(SPISettings s) {
	tcr_base = 0u;
	if (s.dataMode & 0x2) tcr_base |= TCR_CPOL;
	if (s.dataMode & 0x1) tcr_base |= TCR_CPHA;
	if (s.bitOrder == LSBFIRST) tcr_base |= TCR_LSBF;
	setClockDivider(s.clock);                    // adds PRESCALE bits to tcr_base
}

void SPIClass::endTransaction() { /* manual CS; nothing to release */ }

uint8_t SPIClass::transfer(uint8_t data) {
	hw->tcr = tcr_base | TCR_FRAMESZ(7);         // 8-bit frame
	hw->tdr = data;
	for (uint32_t g = 0; g < SPI_TIMEOUT; g++) {
		if (!(hw->rsr & RSR_RXEMPTY)) return (uint8_t)hw->rdr;
	}
	return 0xFFu;
}

uint16_t SPIClass::transfer16(uint16_t data) {
	hw->tcr = tcr_base | TCR_FRAMESZ(15);        // 16-bit frame
	hw->tdr = data;
	for (uint32_t g = 0; g < SPI_TIMEOUT; g++) {
		if (!(hw->rsr & RSR_RXEMPTY)) return (uint16_t)hw->rdr;
	}
	return 0xFFFFu;
}

void SPIClass::transfer(void *buf, size_t count) {
	uint8_t *p = (uint8_t *)buf;
	for (size_t i = 0; i < count; i++) p[i] = transfer(p[i]);
}
