#ifndef Wire_h
#define Wire_h

#include <stdint.h>
#include <stddef.h>
#include "imxrt1176.h"
#include "core_pins.h"   // IRQ_NUMBER_t, attachInterruptVector

#define BUFFER_LENGTH 32
#define WIRE_HAS_END  1

class TwoWire {
public:
	struct hardware_t {
		uint8_t  instance;                       // 0-based LPI2C index (LPI2C1 -> 0)
		IRQ_NUMBER_t irq;
		volatile uint32_t &lpcg;                 // CCM->LPCG[n].DIRECT
		volatile uint32_t &clock_root;           // CCM->CLOCK_ROOT[n].CONTROL
		uint32_t  clock_root_val;                // 0 => 24 MHz (OscRC48MDiv2)
		volatile uint32_t &scl_mux;  uint32_t scl_mux_val;  volatile uint32_t &scl_pad;
		volatile uint32_t &sda_mux;  uint32_t sda_mux_val;  volatile uint32_t &sda_pad;
		volatile uint32_t &scl_select_input; uint32_t scl_select_val;
		volatile uint32_t &sda_select_input; uint32_t sda_select_val;
		uint32_t  pad_ctl_val;                   // open-drain pad config
		volatile uint32_t &mcr;
		volatile uint32_t &msr;
		volatile uint32_t &mcfgr1;
		volatile uint32_t &mccr0;
		volatile uint32_t &mtdr;
		volatile uint32_t &mrdr;
	};

	TwoWire(const hardware_t *hw) : hw(hw) {}

	void begin();
	void end();
	void setClock(uint32_t freq);

	void beginTransmission(uint8_t address) { tx_addr = address; tx_len = 0; }
	size_t write(uint8_t data);
	size_t write(const uint8_t *data, size_t len);
	uint8_t endTransmission(bool sendStop = true);

	uint8_t requestFrom(uint8_t address, uint8_t quantity, bool sendStop = true);
	int available() { return rx_len - rx_idx; }
	int read();
	int peek();

private:
	const hardware_t *hw;
	uint8_t tx_addr = 0;
	uint8_t tx_buf[BUFFER_LENGTH];
	uint8_t tx_len = 0;
	uint8_t rx_buf[BUFFER_LENGTH];
	uint8_t rx_len = 0;
	uint8_t rx_idx = 0;
	uint32_t clock_hz = 100000;

	bool wait_flag(uint32_t mask, uint32_t error_mask, uint32_t &err);
};

extern TwoWire Wire;

#endif
