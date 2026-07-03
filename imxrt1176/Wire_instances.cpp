#include "Wire.h"

static void wire_isr();    // forward decls (defined after the objects)
static void wire1_isr();

/* EVKB Arduino-header I2C is LPI2C1 on GPIO_AD_09 (SDA) / GPIO_AD_08 (SCL), ALT1
 * (confirmed via NXP/Zephyr board docs; LPI2C5/GPIO_LPSR is the onboard eCompass
 * bus, NOT the header). Pads need SION (mux bit4 = 0x10) so the master can sense
 * the bus lines, plus ODE (pad bit4 = 0x10) + DSE drive (pad bit1 = 0x2) for
 * open-drain I2C. Without SION the master reads the bus as perpetually busy. */
static const TwoWire::hardware_t lpi2c1_hw = {
	/* instance */ 0,
	/* irq */ IRQ_LPI2C1,
	/* lpcg */ CCM_LPCG98_DIRECT,
	/* clock_root */ CCM_CLOCK_ROOT37_CONTROL,
	/* clock_root_val */ 0u,                 // OscRC48MDiv2 = 24 MHz
	/* scl_mux */ IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_08, /* scl_mux_val */ 0x11u, // ALT1 | SION
	/* scl_pad */ IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_08,
	/* sda_mux */ IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_09, /* sda_mux_val */ 0x11u, // ALT1 | SION
	/* sda_pad */ IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_09,
	/* scl_select_input */ IOMUXC_LPI2C1_SCL_SELECT_INPUT, /* scl_select_val */ 0u,
	/* sda_select_input */ IOMUXC_LPI2C1_SDA_SELECT_INPUT, /* sda_select_val */ 0u,
	/* pad_ctl_val */ 0x0000001Eu,           // ODE(bit4)|DSE(bit1)|PUE(bit2)|PUS(bit3,up): internal pull-up for header w/o external R
	/* mcr */ LPI2C1_MCR, /* msr */ LPI2C1_MSR, /* mcfgr1 */ LPI2C1_MCFGR1,
	/* mccr0 */ LPI2C1_MCCR0, /* mtdr */ LPI2C1_MTDR, /* mrdr */ LPI2C1_MRDR,
	/* irq_handler */ wire_isr, /* irq_priority */ 16u,
	/* scr */ LPI2C1_SCR, /* ssr */ LPI2C1_SSR, /* sier */ LPI2C1_SIER,
	/* samr */ LPI2C1_SAMR, /* sasr */ LPI2C1_SASR, /* scfgr1 */ LPI2C1_SCFGR1, /* scfgr2 */ LPI2C1_SCFGR2,
	/* stdr */ LPI2C1_STDR, /* srdr */ LPI2C1_SRDR,
};

/* LPI2C2: QEMU-loopback slave only (no physical EVKB pins). Pin/pad refs bind to
 * LPI2C1's IOMUXC regs (inert in QEMU; LPI2C2 is never used on hardware). */
static const TwoWire::hardware_t lpi2c2_hw = {
	/* instance */ 1, /* irq */ IRQ_LPI2C2,
	/* lpcg */ CCM_LPCG99_DIRECT, /* clock_root */ CCM_CLOCK_ROOT38_CONTROL, /* clock_root_val */ 0u,
	/* scl_mux */ IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_08, /* scl_mux_val */ 0x11u, /* scl_pad */ IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_08,
	/* sda_mux */ IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_09, /* sda_mux_val */ 0x11u, /* sda_pad */ IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_09,
	/* scl_select_input */ IOMUXC_LPI2C1_SCL_SELECT_INPUT, /* scl_select_val */ 0u,
	/* sda_select_input */ IOMUXC_LPI2C1_SDA_SELECT_INPUT, /* sda_select_val */ 0u,
	/* pad_ctl_val */ 0x1Eu,
	/* mcr */ LPI2C2_MCR, /* msr */ LPI2C2_MSR, /* mcfgr1 */ LPI2C2_MCFGR1,
	/* mccr0 */ LPI2C2_MCCR0, /* mtdr */ LPI2C2_MTDR, /* mrdr */ LPI2C2_MRDR,
	/* irq_handler */ wire1_isr, /* irq_priority */ 16u,
	/* scr */ LPI2C2_SCR, /* ssr */ LPI2C2_SSR, /* sier */ LPI2C2_SIER,
	/* samr */ LPI2C2_SAMR, /* sasr */ LPI2C2_SASR, /* scfgr1 */ LPI2C2_SCFGR1, /* scfgr2 */ LPI2C2_SCFGR2,
	/* stdr */ LPI2C2_STDR, /* srdr */ LPI2C2_SRDR,
};

TwoWire Wire(&lpi2c1_hw);
TwoWire Wire1(&lpi2c2_hw);

__attribute__((section(".fastrun"))) static void wire_isr()  { Wire.handle_slave_isr(); }
__attribute__((section(".fastrun"))) static void wire1_isr() { Wire1.handle_slave_isr(); }
