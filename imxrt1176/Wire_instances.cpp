#include "Wire.h"

static const TwoWire::hardware_t lpi2c1_hw = {
	/* instance */ 0,
	/* irq */ IRQ_LPI2C1,
	/* lpcg */ CCM_LPCG98_DIRECT,
	/* clock_root */ CCM_CLOCK_ROOT37_CONTROL,
	/* clock_root_val */ 0u,                 // OscRC48MDiv2 = 24 MHz
	/* scl_mux */ IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_08, /* scl_mux_val */ 1u,
	/* scl_pad */ IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_08,
	/* sda_mux */ IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_09, /* sda_mux_val */ 1u,
	/* sda_pad */ IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_09,
	/* scl_select_input */ IOMUXC_LPI2C1_SCL_SELECT_INPUT, /* scl_select_val */ 0u,
	/* sda_select_input */ IOMUXC_LPI2C1_SDA_SELECT_INPUT, /* sda_select_val */ 0u,
	/* pad_ctl_val */ 0x0000000Cu,           // provisional open-drain; HW task finalizes ODE
	/* mcr */ LPI2C1_MCR, /* msr */ LPI2C1_MSR, /* mcfgr1 */ LPI2C1_MCFGR1,
	/* mccr0 */ LPI2C1_MCCR0, /* mtdr */ LPI2C1_MTDR, /* mrdr */ LPI2C1_MRDR,
};

TwoWire Wire(&lpi2c1_hw);
