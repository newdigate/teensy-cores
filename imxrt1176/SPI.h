#ifndef SPI_h
#define SPI_h

#include <stdint.h>
#include <stddef.h>
#include "imxrt1176.h"
#include "core_pins.h"

#define MSBFIRST 1
#define LSBFIRST 0
#define SPI_MODE0 0x00   // CPOL=0, CPHA=0
#define SPI_MODE1 0x01   // CPOL=0, CPHA=1
#define SPI_MODE2 0x02   // CPOL=1, CPHA=0
#define SPI_MODE3 0x03   // CPOL=1, CPHA=1

class SPISettings {
public:
	SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
		: clock(clock), bitOrder(bitOrder), dataMode(dataMode) {}
	SPISettings() : clock(4000000), bitOrder(MSBFIRST), dataMode(SPI_MODE0) {}
	uint32_t clock;
	uint8_t  bitOrder;
	uint8_t  dataMode;
};

class SPIClass {
public:
	struct hardware_t {
		volatile uint32_t &cr;
		volatile uint32_t &cfgr1;
		volatile uint32_t &ccr;
		volatile uint32_t &tcr;
		volatile uint32_t &tdr;
		volatile uint32_t &rsr;
		volatile uint32_t &rdr;
		volatile uint32_t &der;
		volatile uint32_t &fcr;
		volatile uint32_t &lpcg;
		volatile uint32_t &clock_root;
		uint32_t clock_root_val;
		volatile uint32_t &sck_mux;  uint32_t sck_mux_val;  volatile uint32_t &sck_pad;
		volatile uint32_t &sdo_mux;  uint32_t sdo_mux_val;  volatile uint32_t &sdo_pad;
		volatile uint32_t &sdi_mux;  uint32_t sdi_mux_val;  volatile uint32_t &sdi_pad;
		volatile uint32_t &sck_select_input; uint32_t sck_select_val;
		volatile uint32_t &sdo_select_input; uint32_t sdo_select_val;
		volatile uint32_t &sdi_select_input; uint32_t sdi_select_val;
		uint32_t pad_ctl_val;
	};

	SPIClass(const hardware_t *hw) : hw(hw) {}

	void begin();
	void end();
	void beginTransaction(SPISettings settings);
	void endTransaction();
	uint8_t  transfer(uint8_t data);
	uint16_t transfer16(uint16_t data);
	void     transfer(void *buf, size_t count);

private:
	const hardware_t *hw;
	uint32_t tcr_base = 0;
	uint32_t func_clock = 24000000;
	void setClockDivider(uint32_t clockHz);
};

extern SPIClass SPI;

#endif
