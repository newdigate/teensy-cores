#include "SPI.h"

/* EVKB Arduino-header SPI is LPSPI1: SCK=GPIO_AD_28, SOUT/MOSI=GPIO_AD_30,
 * SIN/MISO=GPIO_AD_31, all ALT mode 0. PCS0/CS=GPIO_AD_29 is left as a plain
 * GPIO (manual chip-select, Arduino convention). Push-pull pads (no open-drain,
 * no pull-ups): DSE drive enabled. */
static const SPIClass::hardware_t lpspi1_hw = {
	/* cr */ LPSPI1_CR, /* sr */ LPSPI1_SR, /* cfgr1 */ LPSPI1_CFGR1, /* ccr */ LPSPI1_CCR,
	/* tcr */ LPSPI1_TCR, /* tdr */ LPSPI1_TDR, /* rsr */ LPSPI1_RSR, /* rdr */ LPSPI1_RDR,
	/* lpcg */ CCM_LPCG104_DIRECT, /* clock_root */ CCM_CLOCK_ROOT43_CONTROL, /* clock_root_val */ 0u,
	/* sck */ IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_28, 0x0u, IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_28,
	/* sdo */ IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_30, 0x0u, IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_30,
	/* sdi */ IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_31, 0x0u, IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_31,
	/* sck_select */ IOMUXC_LPSPI1_SCK_SELECT_INPUT, 0x1u,
	/* sdo_select */ IOMUXC_LPSPI1_SDO_SELECT_INPUT, 0x1u,
	/* sdi_select */ IOMUXC_LPSPI1_SDI_SELECT_INPUT, 0x1u,
	/* pad_ctl_val */ 0x0000000Cu,
};

SPIClass SPI(&lpspi1_hw);
