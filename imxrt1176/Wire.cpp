#include "Wire.h"

// MSR flags (imxrt_lpi2c.c contract)
#define MSR_TDF  (1u<<0)
#define MSR_RDF  (1u<<1)
#define MSR_EPF  (1u<<8)
#define MSR_SDF  (1u<<9)
#define MSR_NDF  (1u<<10)
#define MSR_ALF  (1u<<11)
#define MSR_FEF  (1u<<12)
// MCR flags
#define MCR_MEN  (1u<<0)
#define MCR_RST  (1u<<1)
#define MCR_RTF  (1u<<8)
#define MCR_RRF  (1u<<9)
// MTDR commands (data in [7:0], cmd in [10:8])
#define TX_CMD(cmd, data)  (((uint32_t)(cmd) << 8) | ((data) & 0xFF))
#define CMD_TXD    0u
#define CMD_RXD    1u
#define CMD_STOP   2u
#define CMD_START  4u
#define MRDR_RXEMPTY (1u<<14)

#define WIRE_TIMEOUT 100000u   // bounded guard-loop (like analogRead)

void TwoWire::begin() {
	hw->lpcg = 1u;                                     // ungate LPI2C clock
	hw->clock_root = hw->clock_root_val;              // 24 MHz functional clock
	hw->scl_mux = hw->scl_mux_val;  hw->scl_pad = hw->pad_ctl_val;
	hw->sda_mux = hw->sda_mux_val;  hw->sda_pad = hw->pad_ctl_val;
	hw->scl_select_input = hw->scl_select_val;
	hw->sda_select_input = hw->sda_select_val;
	hw->mcr = MCR_RST;                                 // reset the master block
	hw->mcr = 0u;
	setClock(clock_hz);                                // program MCCR0/MCFGR1
	hw->mcr = MCR_MEN;                                 // enable
}

void TwoWire::end() { hw->mcr = 0u; hw->lpcg = 0u; }

void TwoWire::setClock(uint32_t freq) {
	clock_hz = freq;
	const uint32_t src = 24000000u;
	uint32_t pre = 0, div = 0;
	for (pre = 0; pre < 8u; pre++) { div = (src >> pre) / freq; if (div <= 120u) break; }
	uint32_t clklo = (div * 6u) / 10u;                // ~60% low time (I2C tLOW>tHIGH)
	uint32_t clkhi = (div > clklo) ? (div - clklo) : 1u;
	if (clklo > 63u) clklo = 63u;
	if (clkhi > 63u) clkhi = 63u; if (clkhi < 1u) clkhi = 1u;
	uint32_t men = hw->mcr & MCR_MEN;
	hw->mcr = men & ~MCR_MEN;                          // MCCR/MCFGR need MEN=0
	hw->mcfgr1 = (hw->mcfgr1 & ~0x7u) | (pre & 0x7u);
	hw->mccr0 = (clklo) | (clkhi << 8) | ((clkhi/2u) << 16) | ((clkhi/2u) << 24);
	if (men) hw->mcr = MCR_MEN;
}

// Wait until any bit in `mask` is set, or an error bit appears / timeout.
// Returns true on success. On error/timeout sets err to a nonzero Arduino status.
bool TwoWire::wait_flag(uint32_t mask, uint32_t error_mask, uint32_t &err) {
	for (uint32_t g = 0; g < WIRE_TIMEOUT; g++) {
		uint32_t s = hw->msr;
		if (s & error_mask) {
			if (s & MSR_NDF) err = (err == 0xFFu) ? 2u : 3u; // addr vs data NACK
			else err = 4u;                                    // ALF/FEF/other
			hw->msr = s;                                      // W1C the flags
			return false;
		}
		if (s & mask) return true;
	}
	err = 5u;                                                 // timeout
	return false;
}

size_t TwoWire::write(uint8_t data) {
	if (tx_len >= BUFFER_LENGTH) return 0;
	tx_buf[tx_len++] = data; return 1;
}
size_t TwoWire::write(const uint8_t *data, size_t len) {
	size_t n = 0; while (n < len && write(data[n])) n++; return n;
}

uint8_t TwoWire::endTransmission(bool sendStop) {
	uint32_t err = 0xFFu;                                     // 0xFF => next NACK is address NACK
	hw->msr = hw->msr;                                        // clear stale flags
	hw->mtdr = TX_CMD(CMD_START, (tx_addr << 1) | 0u);        // START + addr(W)
	if (!wait_flag(MSR_TDF, MSR_NDF | MSR_ALF | MSR_FEF, err)) { if (sendStop) hw->mtdr = TX_CMD(CMD_STOP,0); return (uint8_t)err; }
	err = 0u;                                                 // address ACKed; further NACK = data NACK (3)
	for (uint8_t i = 0; i < tx_len; i++) {
		hw->mtdr = TX_CMD(CMD_TXD, tx_buf[i]);
		if (!wait_flag(MSR_TDF, MSR_NDF | MSR_ALF | MSR_FEF, err)) { if (sendStop) hw->mtdr = TX_CMD(CMD_STOP,0); return (uint8_t)err; }
	}
	if (sendStop) {
		hw->mtdr = TX_CMD(CMD_STOP, 0);
		wait_flag(MSR_SDF, MSR_ALF | MSR_FEF, err);
		hw->msr = MSR_SDF | MSR_EPF;
	}
	tx_len = 0;
	return 0u;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, bool sendStop) {
	if (quantity > BUFFER_LENGTH) quantity = BUFFER_LENGTH;
	rx_len = 0; rx_idx = 0;
	uint32_t err = 0xFFu;
	hw->msr = hw->msr;
	hw->mtdr = TX_CMD(CMD_START, (address << 1) | 1u);        // START + addr(R)
	if (!wait_flag(MSR_TDF, MSR_NDF | MSR_ALF | MSR_FEF, err)) { if (sendStop) hw->mtdr = TX_CMD(CMD_STOP,0); return 0; }
	hw->mtdr = TX_CMD(CMD_RXD, (uint8_t)(quantity - 1));      // receive `quantity` bytes (N-1 encoding)
	for (uint8_t i = 0; i < quantity; i++) {
		err = 0u;
		if (!wait_flag(MSR_RDF, MSR_ALF | MSR_FEF, err)) break;
		uint32_t r = hw->mrdr;
		if (r & MRDR_RXEMPTY) break;
		rx_buf[rx_len++] = (uint8_t)(r & 0xFF);
	}
	if (sendStop) {
		hw->mtdr = TX_CMD(CMD_STOP, 0);
		wait_flag(MSR_SDF, MSR_ALF | MSR_FEF, err);
		hw->msr = MSR_SDF | MSR_EPF;
	}
	return rx_len;
}

int TwoWire::read()  { return (rx_idx < rx_len) ? rx_buf[rx_idx++] : -1; }
int TwoWire::peek()  { return (rx_idx < rx_len) ? rx_buf[rx_idx]   : -1; }
