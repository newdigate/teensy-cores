// teensy4/semc.c - MIMXRT1060-EVKB SEMC SDRAM bring-up.
// 32 MB SDRAM (ISSI IS42S16160J-class) @ 0x80000000: 16-bit, CS0, 9-col, BL8, CAS3.
// Clocked from PLL2 PFD2 (396 MHz, left untouched - it also clocks the boot flash) / 3
// = 132 MHz. Register values dumped from NXP's semc_sdram example reconfigured to 132 MHz.
// MCR bit layout: bit0=SWRST, bit1=MDIS, bit2=DQSMD. DQSMD MUST be 0 (internal read-strobe
// loopback); the SDK's DQSMD=1 (DQS-pad loopback) returns all-zero reads on this board.
#if defined(ARDUINO_MIMXRT1060_EVKB)
#include "imxrt.h"
#include "avr/pgmspace.h"  // FLASHMEM
#include "semc.h"

// One SEMC IP command (cmd low nibble: 0xF=precharge-all, 0xC=auto-refresh, 0xA=mode-set).
// Bounded poll so a non-responding controller (e.g. a QEMU stub) can't hang boot.
static void semc_ipcmd(uint32_t addr, uint32_t cmd, uint32_t data)
{
	SEMC_INTR = 0x1;                       // clear IPCMDDONE (write-1-clear)
	SEMC_IPCR0 = addr;
	uint32_t m = cmd & 0xF;
	if (m == 0xA || m == 0x9) SEMC_IPTXDAT = data;   // mode-set / write carry data
	SEMC_IPCMD = cmd | (0xA55Au << 16);    // SEMC_IPCMD magic key
	uint32_t guard = 2000000;
	while (!(SEMC_INTR & 0x1) && --guard) { }
}

FLASHMEM void extram_semc_init(void)
{
	// Pins: EMC_00..EMC_41 -> ALT0 (SEMC), strong/fast pads (HYS|keeper|200MHz|DSE7|fast).
	volatile uint32_t *mux = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_00;
	volatile uint32_t *pad = &IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_00;
	for (int i = 0; i < 42; i++) { mux[i] = 0; pad[i] = 0x110F9; }

	// Clock: SEMC = PLL2 PFD2 / 3 = 132 MHz. PFD2 untouched. SEMC_PODF needs the handshake.
	CCM_CBCDR = (CCM_CBCDR & ~CCM_CBCDR_SEMC_ALT_CLK_SEL) | CCM_CBCDR_SEMC_CLK_SEL; // alt=PFD2
	CCM_CBCDR = (CCM_CBCDR & ~CCM_CBCDR_SEMC_PODF(7)) | CCM_CBCDR_SEMC_PODF(2);     // /3
	uint32_t hg = 2000000;
	while ((CCM_CDHIPR & CCM_CDHIPR_SEMC_PODF_BUSY) && --hg) { }
	CCM_CCGR3 |= CCM_CCGR3_SEMC(3);                                                 // gate on

	// Soft reset (SWRST=bit0), then configure disabled (MDIS=1) and enable (MDIS=0); DQSMD=0.
	SEMC_MCR = 0x1u;
	uint32_t guard = 2000000;
	while ((SEMC_MCR & 0x1u) && --guard) { }
	SEMC_MCR   = 0x1FFF0002;   // MDIS=1, DQSMD=0, BTO/CTO
	SEMC_BMCR0 = 0x00104085;
	SEMC_BMCR1 = 0x40246085;
	SEMC_MCR   = 0x1FFF0000;   // enable (MDIS=0), DQSMD=0
	SEMC_IOCR     = 0x00000000;
	SEMC_INTEN    = 0x00000000;
	SEMC_BR0      = 0x8000001B; // base 0x80000000, 32 MB, valid
	SEMC_SDRAMCR0 = 0x00000F31; // 16-bit, BL8, 9-col, CAS3
	SEMC_SDRAMCR1 = 0x00440711; // timings @132 MHz
	SEMC_SDRAMCR2 = 0x00060607;
	SEMC_SDRAMCR3 = 0x06050A00; // refresh disabled (REN=0) during init
	SEMC_IPCR1    = 0x00000002;

	// JEDEC power-up: >100us stable clock before init commands. Busy-loop because delay()/
	// the DWT cycle counter are not running yet this early in boot; this is >=100us at any
	// core clock (200k NOPs ~= 0.67 ms at 600 MHz, more when slower).
	for (volatile uint32_t w = 0; w < 200000; w++) { __asm__ volatile("nop"); }

	semc_ipcmd(0x80000000UL, 0xF, 0);                                  // precharge all
	for (int r = 0; r < 8; r++) semc_ipcmd(0x80000000UL, 0xC, 0);      // auto-refresh x8
	semc_ipcmd(0x80000000UL, 0xA, 0x33);                               // mode-set: BL8 | CAS3

	SEMC_SDRAMCR3 = 0x06050A01; // enable auto-refresh (REN=1)
}
#endif // ARDUINO_MIMXRT1060_EVKB
