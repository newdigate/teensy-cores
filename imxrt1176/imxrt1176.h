/* Standalone Teensy-idiom register definitions for the NXP i.MX RT1176 (CM7).
 *
 * HAND-MAINTAINED -- edit this file directly.
 *
 * This header was originally bootstrapped by tools/gen_imxrt1176_h.py, but it
 * has since been hand-edited for many peripheral additions that were never
 * mirrored back into that script (e.g. ADC_CMDL_ABSEL, the DAC12 block's
 * position, and the whole PXP block).  The header -- not the script -- is now
 * the single source of truth.  Add new peripherals here directly, following
 * the existing per-peripheral comment-banner style ("--- <PERIPHERAL> ---").
 *
 * The generator is RETIRED and archived under tools/attic/ (see
 * tools/attic/README.md).  Do NOT run it against this file: it would silently
 * drop the hand-added definitions above and break the build. */
#ifndef _imxrt1176_h_
#define _imxrt1176_h_
#include <stdint.h>

#define ANADIG_OSC_BASE 0x40C84000u
#define ANADIG_PLL_BASE 0x40C84000u
#define CCM_BASE 0x40CC0000u
#define ENET_BASE 0x40424000u
#define FLEXSPI_BASE 0x400CC000u
#define FLEXSPI2_BASE 0x400D0000u
#define GPIO1_BASE 0x4012C000u
#define GPIO10_BASE 0x40C68000u
#define GPIO11_BASE 0x40C6C000u
#define GPIO12_BASE 0x40C70000u
#define GPIO13_BASE 0x40CA0000u
#define GPIO2_BASE 0x40130000u
#define GPIO3_BASE 0x40134000u
#define GPIO4_BASE 0x40138000u
#define GPIO5_BASE 0x4013C000u
#define GPIO6_BASE 0x40140000u
#define GPIO7_BASE 0x40C5C000u
#define GPIO8_BASE 0x40C60000u
#define GPIO9_BASE 0x40C64000u
#define IOMUXC_BASE 0x400E8000u
#define IOMUXC_GPR_BASE 0x400E4000u
#define GPIO1_DR        (*(volatile uint32_t *)0x4012C000u)
#define GPIO1_GDIR      (*(volatile uint32_t *)0x4012C004u)
#define GPIO1_PSR       (*(volatile uint32_t *)0x4012C008u)
#define GPIO1_DR_SET    (*(volatile uint32_t *)0x4012C084u)
#define GPIO1_DR_CLEAR  (*(volatile uint32_t *)0x4012C088u)
#define GPIO1_DR_TOGGLE (*(volatile uint32_t *)0x4012C08Cu)
#define GPIO2_DR        (*(volatile uint32_t *)0x40130000u)
#define GPIO2_GDIR      (*(volatile uint32_t *)0x40130004u)
#define GPIO2_PSR       (*(volatile uint32_t *)0x40130008u)
#define GPIO2_DR_SET    (*(volatile uint32_t *)0x40130084u)
#define GPIO2_DR_CLEAR  (*(volatile uint32_t *)0x40130088u)
#define GPIO2_DR_TOGGLE (*(volatile uint32_t *)0x4013008Cu)
#define GPIO3_DR        (*(volatile uint32_t *)0x40134000u)
#define GPIO3_GDIR      (*(volatile uint32_t *)0x40134004u)
#define GPIO3_PSR       (*(volatile uint32_t *)0x40134008u)
#define GPIO3_DR_SET    (*(volatile uint32_t *)0x40134084u)
#define GPIO3_DR_CLEAR  (*(volatile uint32_t *)0x40134088u)
#define GPIO3_DR_TOGGLE (*(volatile uint32_t *)0x4013408Cu)
#define GPIO4_DR        (*(volatile uint32_t *)0x40138000u)
#define GPIO4_GDIR      (*(volatile uint32_t *)0x40138004u)
#define GPIO4_PSR       (*(volatile uint32_t *)0x40138008u)
#define GPIO4_DR_SET    (*(volatile uint32_t *)0x40138084u)
#define GPIO4_DR_CLEAR  (*(volatile uint32_t *)0x40138088u)
#define GPIO4_DR_TOGGLE (*(volatile uint32_t *)0x4013808Cu)
#define GPIO5_DR        (*(volatile uint32_t *)0x4013C000u)
#define GPIO5_GDIR      (*(volatile uint32_t *)0x4013C004u)
#define GPIO5_PSR       (*(volatile uint32_t *)0x4013C008u)
#define GPIO5_DR_SET    (*(volatile uint32_t *)0x4013C084u)
#define GPIO5_DR_CLEAR  (*(volatile uint32_t *)0x4013C088u)
#define GPIO5_DR_TOGGLE (*(volatile uint32_t *)0x4013C08Cu)
#define GPIO6_DR        (*(volatile uint32_t *)0x40140000u)
#define GPIO6_GDIR      (*(volatile uint32_t *)0x40140004u)
#define GPIO6_PSR       (*(volatile uint32_t *)0x40140008u)
#define GPIO6_DR_SET    (*(volatile uint32_t *)0x40140084u)
#define GPIO6_DR_CLEAR  (*(volatile uint32_t *)0x40140088u)
#define GPIO6_DR_TOGGLE (*(volatile uint32_t *)0x4014008Cu)
#define GPIO7_DR        (*(volatile uint32_t *)0x40C5C000u)
#define GPIO7_GDIR      (*(volatile uint32_t *)0x40C5C004u)
#define GPIO7_PSR       (*(volatile uint32_t *)0x40C5C008u)
#define GPIO7_DR_SET    (*(volatile uint32_t *)0x40C5C084u)
#define GPIO7_DR_CLEAR  (*(volatile uint32_t *)0x40C5C088u)
#define GPIO7_DR_TOGGLE (*(volatile uint32_t *)0x40C5C08Cu)
#define GPIO8_DR        (*(volatile uint32_t *)0x40C60000u)
#define GPIO8_GDIR      (*(volatile uint32_t *)0x40C60004u)
#define GPIO8_PSR       (*(volatile uint32_t *)0x40C60008u)
#define GPIO8_DR_SET    (*(volatile uint32_t *)0x40C60084u)
#define GPIO8_DR_CLEAR  (*(volatile uint32_t *)0x40C60088u)
#define GPIO8_DR_TOGGLE (*(volatile uint32_t *)0x40C6008Cu)
#define GPIO9_DR        (*(volatile uint32_t *)0x40C64000u)
#define GPIO9_GDIR      (*(volatile uint32_t *)0x40C64004u)
#define GPIO9_PSR       (*(volatile uint32_t *)0x40C64008u)
#define GPIO9_DR_SET    (*(volatile uint32_t *)0x40C64084u)
#define GPIO9_DR_CLEAR  (*(volatile uint32_t *)0x40C64088u)
#define GPIO9_DR_TOGGLE (*(volatile uint32_t *)0x40C6408Cu)
#define GPIO10_DR        (*(volatile uint32_t *)0x40C68000u)
#define GPIO10_GDIR      (*(volatile uint32_t *)0x40C68004u)
#define GPIO10_PSR       (*(volatile uint32_t *)0x40C68008u)
#define GPIO10_DR_SET    (*(volatile uint32_t *)0x40C68084u)
#define GPIO10_DR_CLEAR  (*(volatile uint32_t *)0x40C68088u)
#define GPIO10_DR_TOGGLE (*(volatile uint32_t *)0x40C6808Cu)
#define GPIO11_DR        (*(volatile uint32_t *)0x40C6C000u)
#define GPIO11_GDIR      (*(volatile uint32_t *)0x40C6C004u)
#define GPIO11_PSR       (*(volatile uint32_t *)0x40C6C008u)
#define GPIO11_DR_SET    (*(volatile uint32_t *)0x40C6C084u)
#define GPIO11_DR_CLEAR  (*(volatile uint32_t *)0x40C6C088u)
#define GPIO11_DR_TOGGLE (*(volatile uint32_t *)0x40C6C08Cu)
#define GPIO12_DR        (*(volatile uint32_t *)0x40C70000u)
#define GPIO12_GDIR      (*(volatile uint32_t *)0x40C70004u)
#define GPIO12_PSR       (*(volatile uint32_t *)0x40C70008u)
#define GPIO12_DR_SET    (*(volatile uint32_t *)0x40C70084u)
#define GPIO12_DR_CLEAR  (*(volatile uint32_t *)0x40C70088u)
#define GPIO12_DR_TOGGLE (*(volatile uint32_t *)0x40C7008Cu)
#define GPIO13_DR        (*(volatile uint32_t *)0x40CA0000u)
#define GPIO13_GDIR      (*(volatile uint32_t *)0x40CA0004u)
#define GPIO13_PSR       (*(volatile uint32_t *)0x40CA0008u)
#define GPIO13_DR_SET    (*(volatile uint32_t *)0x40CA0084u)
#define GPIO13_DR_CLEAR  (*(volatile uint32_t *)0x40CA0088u)
#define GPIO13_DR_TOGGLE (*(volatile uint32_t *)0x40CA008Cu)

/* CCM clock roots: CCM->CLOCK_ROOT[n].CONTROL @ CCM_BASE + n*0x80 */
#define CCM_CLOCK_ROOT0_CONTROL (*(volatile uint32_t *)0x40CC0000u) /* M7 */
#define CCM_CLOCK_ROOT2_CONTROL (*(volatile uint32_t *)0x40CC0100u) /* BUS */
#define CCM_CLOCK_ROOT8_CONTROL (*(volatile uint32_t *)0x40CC0400u) /* M7_SYSTICK */
#define CCM_CLOCK_ROOT_CONTROL_MUX(x)  (((uint32_t)(x) << 8) & 0x700u)
#define CCM_CLOCK_ROOT_CONTROL_DIV(x)  (((uint32_t)(x) << 0) & 0x0FFu)

#define ANADIG_PLL_ARM_PLL_CTRL (*(volatile uint32_t *)0x40C84200u)
#define ANADIG_PLL_ARM_PLL_CTRL_DIV_SELECT(x)   (((uint32_t)(x) << 0) & 0x000000FFu)
#define ANADIG_PLL_ARM_PLL_CTRL_HOLD_RING_OFF   (1u << 12)
#define ANADIG_PLL_ARM_PLL_CTRL_POWERUP         (1u << 13)
#define ANADIG_PLL_ARM_PLL_CTRL_ENABLE_CLK      (1u << 14)
#define ANADIG_PLL_ARM_PLL_CTRL_POST_DIV_SEL(x) (((uint32_t)(x) << 15) & 0x00018000u)
#define ANADIG_PLL_ARM_PLL_CTRL_BYPASS          (1u << 17)
#define ANADIG_PLL_ARM_PLL_CTRL_ARM_PLL_STABLE  (1u << 29)
#define ANADIG_PLL_ARM_PLL_CTRL_ARM_PLL_GATE    (1u << 30)
#define ANADIG_OSC_OSC_24M_CTRL (*(volatile uint32_t *)0x40C84020u)
#define ANADIG_OSC_OSC_24M_CTRL_OSC_EN          (1u << 4)
#define ANADIG_OSC_OSC_24M_CTRL_LP_EN           (1u << 2)
#define ANADIG_OSC_OSC_24M_CTRL_OSC_24M_STABLE  (1u << 30)

/* IOMUXC per-pad mux/pad-control regs (offsets from fsl_iomuxc.h) */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_04 (*(volatile uint32_t *)0x400E811Cu)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_06 (*(volatile uint32_t *)0x400E8124u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_24 (*(volatile uint32_t *)0x400E816Cu)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_25 (*(volatile uint32_t *)0x400E8170u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_04 (*(volatile uint32_t *)0x400E8360u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_06 (*(volatile uint32_t *)0x400E8368u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_24 (*(volatile uint32_t *)0x400E83B0u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_25 (*(volatile uint32_t *)0x400E83B4u)

#define SYST_CSR   (*(volatile uint32_t *)0xE000E010u)
#define SYST_RVR   (*(volatile uint32_t *)0xE000E014u)
#define SYST_CVR   (*(volatile uint32_t *)0xE000E018u)
#define SCB_VTOR   (*(volatile uint32_t *)0xE000ED08u)
#define SCB_ICSR   (*(volatile uint32_t *)0xE000ED04u)
#define SCB_ICSR_PENDSTSET (1u << 26)
#define ARM_DWT_CYCCNT (*(volatile uint32_t *)0xE0001004u)
#define ARM_DWT_CTRL   (*(volatile uint32_t *)0xE0001000u)
#define ARM_DEMCR      (*(volatile uint32_t *)0xE000EDFCu)
#define ARM_DEMCR_TRCENA (1u << 24)

/* --- Cortex-M7 NVIC (Task: interrupt infrastructure) --- */
#define NVIC_ISER(n) (*(volatile uint32_t *)(0xE000E100u + ((n) << 2)))
#define NVIC_ICER(n) (*(volatile uint32_t *)(0xE000E180u + ((n) << 2)))
#define NVIC_ISPR(n) (*(volatile uint32_t *)(0xE000E200u + ((n) << 2)))
#define NVIC_ICPR(n) (*(volatile uint32_t *)(0xE000E280u + ((n) << 2)))
#define NVIC_IP(n)   (*(volatile uint8_t  *)(0xE000E400u + (n)))
#define NVIC_ENABLE_IRQ(n)   (NVIC_ISER((n) >> 5) = (1u << ((n) & 31)))
#define NVIC_IS_ENABLED(n)   (NVIC_ISER((n) >> 5) & (1u << ((n) & 31)))
#define NVIC_DISABLE_IRQ(n)  (NVIC_ICER((n) >> 5) = (1u << ((n) & 31)))
#define NVIC_SET_PENDING(n)  (NVIC_ISPR((n) >> 5) = (1u << ((n) & 31)))
#define NVIC_CLEAR_PENDING(n)(NVIC_ICPR((n) >> 5) = (1u << ((n) & 31)))
#define NVIC_SET_PRIORITY(n, p) (NVIC_IP(n) = (uint8_t)(p))

/* --- LPUART register blocks (Task: Serial1) --- */
#define LPUART1_VERID (*(volatile uint32_t *)0x4007C000u)
#define LPUART1_PARAM (*(volatile uint32_t *)0x4007C004u)
#define LPUART1_GLOBAL (*(volatile uint32_t *)0x4007C008u)
#define LPUART1_PINCFG (*(volatile uint32_t *)0x4007C00Cu)
#define LPUART1_BAUD (*(volatile uint32_t *)0x4007C010u)
#define LPUART1_STAT (*(volatile uint32_t *)0x4007C014u)
#define LPUART1_CTRL (*(volatile uint32_t *)0x4007C018u)
#define LPUART1_DATA (*(volatile uint32_t *)0x4007C01Cu)
#define LPUART1_MATCH (*(volatile uint32_t *)0x4007C020u)
#define LPUART1_MODIR (*(volatile uint32_t *)0x4007C024u)
#define LPUART1_FIFO (*(volatile uint32_t *)0x4007C028u)
#define LPUART1_WATER (*(volatile uint32_t *)0x4007C02Cu)
#define LPUART2_VERID (*(volatile uint32_t *)0x40080000u)
#define LPUART2_PARAM (*(volatile uint32_t *)0x40080004u)
#define LPUART2_GLOBAL (*(volatile uint32_t *)0x40080008u)
#define LPUART2_PINCFG (*(volatile uint32_t *)0x4008000Cu)
#define LPUART2_BAUD (*(volatile uint32_t *)0x40080010u)
#define LPUART2_STAT (*(volatile uint32_t *)0x40080014u)
#define LPUART2_CTRL (*(volatile uint32_t *)0x40080018u)
#define LPUART2_DATA (*(volatile uint32_t *)0x4008001Cu)
#define LPUART2_MATCH (*(volatile uint32_t *)0x40080020u)
#define LPUART2_MODIR (*(volatile uint32_t *)0x40080024u)
#define LPUART2_FIFO (*(volatile uint32_t *)0x40080028u)
#define LPUART2_WATER (*(volatile uint32_t *)0x4008002Cu)
#define LPUART3_VERID (*(volatile uint32_t *)0x40084000u)
#define LPUART3_PARAM (*(volatile uint32_t *)0x40084004u)
#define LPUART3_GLOBAL (*(volatile uint32_t *)0x40084008u)
#define LPUART3_PINCFG (*(volatile uint32_t *)0x4008400Cu)
#define LPUART3_BAUD (*(volatile uint32_t *)0x40084010u)
#define LPUART3_STAT (*(volatile uint32_t *)0x40084014u)
#define LPUART3_CTRL (*(volatile uint32_t *)0x40084018u)
#define LPUART3_DATA (*(volatile uint32_t *)0x4008401Cu)
#define LPUART3_MATCH (*(volatile uint32_t *)0x40084020u)
#define LPUART3_MODIR (*(volatile uint32_t *)0x40084024u)
#define LPUART3_FIFO (*(volatile uint32_t *)0x40084028u)
#define LPUART3_WATER (*(volatile uint32_t *)0x4008402Cu)
#define LPUART4_VERID (*(volatile uint32_t *)0x40088000u)
#define LPUART4_PARAM (*(volatile uint32_t *)0x40088004u)
#define LPUART4_GLOBAL (*(volatile uint32_t *)0x40088008u)
#define LPUART4_PINCFG (*(volatile uint32_t *)0x4008800Cu)
#define LPUART4_BAUD (*(volatile uint32_t *)0x40088010u)
#define LPUART4_STAT (*(volatile uint32_t *)0x40088014u)
#define LPUART4_CTRL (*(volatile uint32_t *)0x40088018u)
#define LPUART4_DATA (*(volatile uint32_t *)0x4008801Cu)
#define LPUART4_MATCH (*(volatile uint32_t *)0x40088020u)
#define LPUART4_MODIR (*(volatile uint32_t *)0x40088024u)
#define LPUART4_FIFO (*(volatile uint32_t *)0x40088028u)
#define LPUART4_WATER (*(volatile uint32_t *)0x4008802Cu)
#define LPUART5_VERID (*(volatile uint32_t *)0x4008C000u)
#define LPUART5_PARAM (*(volatile uint32_t *)0x4008C004u)
#define LPUART5_GLOBAL (*(volatile uint32_t *)0x4008C008u)
#define LPUART5_PINCFG (*(volatile uint32_t *)0x4008C00Cu)
#define LPUART5_BAUD (*(volatile uint32_t *)0x4008C010u)
#define LPUART5_STAT (*(volatile uint32_t *)0x4008C014u)
#define LPUART5_CTRL (*(volatile uint32_t *)0x4008C018u)
#define LPUART5_DATA (*(volatile uint32_t *)0x4008C01Cu)
#define LPUART5_MATCH (*(volatile uint32_t *)0x4008C020u)
#define LPUART5_MODIR (*(volatile uint32_t *)0x4008C024u)
#define LPUART5_FIFO (*(volatile uint32_t *)0x4008C028u)
#define LPUART5_WATER (*(volatile uint32_t *)0x4008C02Cu)
#define LPUART6_VERID (*(volatile uint32_t *)0x40090000u)
#define LPUART6_PARAM (*(volatile uint32_t *)0x40090004u)
#define LPUART6_GLOBAL (*(volatile uint32_t *)0x40090008u)
#define LPUART6_PINCFG (*(volatile uint32_t *)0x4009000Cu)
#define LPUART6_BAUD (*(volatile uint32_t *)0x40090010u)
#define LPUART6_STAT (*(volatile uint32_t *)0x40090014u)
#define LPUART6_CTRL (*(volatile uint32_t *)0x40090018u)
#define LPUART6_DATA (*(volatile uint32_t *)0x4009001Cu)
#define LPUART6_MATCH (*(volatile uint32_t *)0x40090020u)
#define LPUART6_MODIR (*(volatile uint32_t *)0x40090024u)
#define LPUART6_FIFO (*(volatile uint32_t *)0x40090028u)
#define LPUART6_WATER (*(volatile uint32_t *)0x4009002Cu)
#define LPUART7_VERID (*(volatile uint32_t *)0x40094000u)
#define LPUART7_PARAM (*(volatile uint32_t *)0x40094004u)
#define LPUART7_GLOBAL (*(volatile uint32_t *)0x40094008u)
#define LPUART7_PINCFG (*(volatile uint32_t *)0x4009400Cu)
#define LPUART7_BAUD (*(volatile uint32_t *)0x40094010u)
#define LPUART7_STAT (*(volatile uint32_t *)0x40094014u)
#define LPUART7_CTRL (*(volatile uint32_t *)0x40094018u)
#define LPUART7_DATA (*(volatile uint32_t *)0x4009401Cu)
#define LPUART7_MATCH (*(volatile uint32_t *)0x40094020u)
#define LPUART7_MODIR (*(volatile uint32_t *)0x40094024u)
#define LPUART7_FIFO (*(volatile uint32_t *)0x40094028u)
#define LPUART7_WATER (*(volatile uint32_t *)0x4009402Cu)
#define LPUART8_VERID (*(volatile uint32_t *)0x40098000u)
#define LPUART8_PARAM (*(volatile uint32_t *)0x40098004u)
#define LPUART8_GLOBAL (*(volatile uint32_t *)0x40098008u)
#define LPUART8_PINCFG (*(volatile uint32_t *)0x4009800Cu)
#define LPUART8_BAUD (*(volatile uint32_t *)0x40098010u)
#define LPUART8_STAT (*(volatile uint32_t *)0x40098014u)
#define LPUART8_CTRL (*(volatile uint32_t *)0x40098018u)
#define LPUART8_DATA (*(volatile uint32_t *)0x4009801Cu)
#define LPUART8_MATCH (*(volatile uint32_t *)0x40098020u)
#define LPUART8_MODIR (*(volatile uint32_t *)0x40098024u)
#define LPUART8_FIFO (*(volatile uint32_t *)0x40098028u)
#define LPUART8_WATER (*(volatile uint32_t *)0x4009802Cu)
#define LPUART9_VERID (*(volatile uint32_t *)0x4009C000u)
#define LPUART9_PARAM (*(volatile uint32_t *)0x4009C004u)
#define LPUART9_GLOBAL (*(volatile uint32_t *)0x4009C008u)
#define LPUART9_PINCFG (*(volatile uint32_t *)0x4009C00Cu)
#define LPUART9_BAUD (*(volatile uint32_t *)0x4009C010u)
#define LPUART9_STAT (*(volatile uint32_t *)0x4009C014u)
#define LPUART9_CTRL (*(volatile uint32_t *)0x4009C018u)
#define LPUART9_DATA (*(volatile uint32_t *)0x4009C01Cu)
#define LPUART9_MATCH (*(volatile uint32_t *)0x4009C020u)
#define LPUART9_MODIR (*(volatile uint32_t *)0x4009C024u)
#define LPUART9_FIFO (*(volatile uint32_t *)0x4009C028u)
#define LPUART9_WATER (*(volatile uint32_t *)0x4009C02Cu)
#define LPUART10_VERID (*(volatile uint32_t *)0x400A0000u)
#define LPUART10_PARAM (*(volatile uint32_t *)0x400A0004u)
#define LPUART10_GLOBAL (*(volatile uint32_t *)0x400A0008u)
#define LPUART10_PINCFG (*(volatile uint32_t *)0x400A000Cu)
#define LPUART10_BAUD (*(volatile uint32_t *)0x400A0010u)
#define LPUART10_STAT (*(volatile uint32_t *)0x400A0014u)
#define LPUART10_CTRL (*(volatile uint32_t *)0x400A0018u)
#define LPUART10_DATA (*(volatile uint32_t *)0x400A001Cu)
#define LPUART10_MATCH (*(volatile uint32_t *)0x400A0020u)
#define LPUART10_MODIR (*(volatile uint32_t *)0x400A0024u)
#define LPUART10_FIFO (*(volatile uint32_t *)0x400A0028u)
#define LPUART10_WATER (*(volatile uint32_t *)0x400A002Cu)
#define LPUART11_VERID (*(volatile uint32_t *)0x40C24000u)
#define LPUART11_PARAM (*(volatile uint32_t *)0x40C24004u)
#define LPUART11_GLOBAL (*(volatile uint32_t *)0x40C24008u)
#define LPUART11_PINCFG (*(volatile uint32_t *)0x40C2400Cu)
#define LPUART11_BAUD (*(volatile uint32_t *)0x40C24010u)
#define LPUART11_STAT (*(volatile uint32_t *)0x40C24014u)
#define LPUART11_CTRL (*(volatile uint32_t *)0x40C24018u)
#define LPUART11_DATA (*(volatile uint32_t *)0x40C2401Cu)
#define LPUART11_MATCH (*(volatile uint32_t *)0x40C24020u)
#define LPUART11_MODIR (*(volatile uint32_t *)0x40C24024u)
#define LPUART11_FIFO (*(volatile uint32_t *)0x40C24028u)
#define LPUART11_WATER (*(volatile uint32_t *)0x40C2402Cu)
#define LPUART12_VERID (*(volatile uint32_t *)0x40C28000u)
#define LPUART12_PARAM (*(volatile uint32_t *)0x40C28004u)
#define LPUART12_GLOBAL (*(volatile uint32_t *)0x40C28008u)
#define LPUART12_PINCFG (*(volatile uint32_t *)0x40C2800Cu)
#define LPUART12_BAUD (*(volatile uint32_t *)0x40C28010u)
#define LPUART12_STAT (*(volatile uint32_t *)0x40C28014u)
#define LPUART12_CTRL (*(volatile uint32_t *)0x40C28018u)
#define LPUART12_DATA (*(volatile uint32_t *)0x40C2801Cu)
#define LPUART12_MATCH (*(volatile uint32_t *)0x40C28020u)
#define LPUART12_MODIR (*(volatile uint32_t *)0x40C28024u)
#define LPUART12_FIFO (*(volatile uint32_t *)0x40C28028u)
#define LPUART12_WATER (*(volatile uint32_t *)0x40C2802Cu)

/* LPUART bitfields (RT1176 RM / PERI_LPUART.h) */
#define LPUART_STAT_RDRF   (1u << 21)
#define LPUART_STAT_TDRE   (1u << 23)
#define LPUART_STAT_TC     (1u << 22)
#define LPUART_STAT_IDLE   (1u << 20)
#define LPUART_STAT_OR     (1u << 19)
#define LPUART_CTRL_RE     (1u << 18)
#define LPUART_CTRL_TE     (1u << 19)
#define LPUART_CTRL_ILIE   (1u << 20)
#define LPUART_CTRL_RIE    (1u << 21)
#define LPUART_CTRL_TCIE   (1u << 22)
#define LPUART_CTRL_TIE    (1u << 23)
#define LPUART_BAUD_SBR(n)   ((uint32_t)(n) & 0x1FFFu)
#define LPUART_BAUD_OSR(n)   (((uint32_t)(n) & 0x1Fu) << 24)
#define LPUART_BAUD_BOTHEDGE (1u << 17)
#define LPUART_FIFO_RXFE   (1u << 3)
#define LPUART_FIFO_TXFE   (1u << 7)
#define LPUART_WATER_TXWATER(n) ((uint32_t)(n) & 0x3u)
#define LPUART_WATER_RXWATER(n) (((uint32_t)(n) << 16) & 0x30000u)

/* LPUART1 console clock + input select (EVKB VCOM) */
#define CCM_CLOCK_ROOT25_CONTROL (*(volatile uint32_t *)0x40CC0C80u)
#define CCM_LPCG86_DIRECT        (*(volatile uint32_t *)0x40CC6AC0u)
#define IOMUXC_LPUART1_TXD_SELECT_INPUT (*(volatile uint32_t *)0x400E8620u)
#define IOMUXC_LPUART1_RXD_SELECT_INPUT (*(volatile uint32_t *)0x400E861Cu)

/* FlexCAN3 (J47): clock root 24 (mux 1 = Osc24MOut, div 1 = 24 MHz) +
 * LPCG 85, and the CAN3 TX/RX pads in the LPSR IOMUXC (0x40C08000). */
#define CCM_CLOCK_ROOT24_CONTROL (*(volatile uint32_t *)0x40CC0C00u)
#define CCM_LPCG85_DIRECT        (*(volatile uint32_t *)0x40CC6AA0u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_LPSR_00 (*(volatile uint32_t *)0x40C08000u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_LPSR_01 (*(volatile uint32_t *)0x40C08004u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_LPSR_00 (*(volatile uint32_t *)0x40C08040u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_LPSR_01 (*(volatile uint32_t *)0x40C08044u)
#define IOMUXC_FLEXCAN3_RX_SELECT_INPUT    (*(volatile uint32_t *)0x40C08080u)

/* --- LPADC register blocks (Task: analogRead) --- */
#define ADC1_VERID (*(volatile uint32_t *)0x40050000u)
#define ADC1_PARAM (*(volatile uint32_t *)0x40050004u)
#define ADC1_CTRL (*(volatile uint32_t *)0x40050010u)
#define ADC1_STAT (*(volatile uint32_t *)0x40050014u)
#define ADC1_IE (*(volatile uint32_t *)0x40050018u)
#define ADC1_DE (*(volatile uint32_t *)0x4005001Cu)
#define ADC1_CFG (*(volatile uint32_t *)0x40050020u)
#define ADC1_PAUSE (*(volatile uint32_t *)0x40050024u)
#define ADC1_FCTRL (*(volatile uint32_t *)0x40050030u)
#define ADC1_SWTRIG (*(volatile uint32_t *)0x40050034u)
#define ADC1_TCTRL0 (*(volatile uint32_t *)0x400500C0u)
#define ADC1_CMDL1 (*(volatile uint32_t *)0x40050100u)
#define ADC1_CMDH1 (*(volatile uint32_t *)0x40050104u)
#define ADC1_RESFIFO (*(volatile uint32_t *)0x40050300u)
#define ADC2_VERID (*(volatile uint32_t *)0x40054000u)
#define ADC2_PARAM (*(volatile uint32_t *)0x40054004u)
#define ADC2_CTRL (*(volatile uint32_t *)0x40054010u)
#define ADC2_STAT (*(volatile uint32_t *)0x40054014u)
#define ADC2_IE (*(volatile uint32_t *)0x40054018u)
#define ADC2_DE (*(volatile uint32_t *)0x4005401Cu)
#define ADC2_CFG (*(volatile uint32_t *)0x40054020u)
#define ADC2_PAUSE (*(volatile uint32_t *)0x40054024u)
#define ADC2_FCTRL (*(volatile uint32_t *)0x40054030u)
#define ADC2_SWTRIG (*(volatile uint32_t *)0x40054034u)
#define ADC2_TCTRL0 (*(volatile uint32_t *)0x400540C0u)
#define ADC2_CMDL1 (*(volatile uint32_t *)0x40054100u)
#define ADC2_CMDH1 (*(volatile uint32_t *)0x40054104u)
#define ADC2_RESFIFO (*(volatile uint32_t *)0x40054300u)

/* LPADC bitfields (RT1176 RM / PERI_ADC.h) */
#define ADC_CTRL_ADCEN   (1u << 0)
#define ADC_CTRL_RST     (1u << 1)
#define ADC_CTRL_RSTFIFO (1u << 8)
#define ADC_STAT_RDY     (1u << 0)
#define ADC_STAT_FOF     (1u << 1)
#define ADC_IE_FWMIE     (1u << 0)
#define ADC_CMDL_ADCH(x)   ((uint32_t)(x) & 0x1Fu)
#define ADC_CMDL_ABSEL     (1u << 5)   /* single-ended: 0 = A-side pad, 1 = B-side pad */
#define ADC_CMDL_CSCALE    (1u << 13)  /* 1 = full scale (SDK default); 0 = input scaled 30/64 */
#define ADC_CMDH_STS(x)    (((uint32_t)(x) & 0x7u) << 8)  /* sample time: 0=3.5 ADCK .. 7=131.5 ADCK */
#define ADC_TCTRL_TCMD(x)  (((uint32_t)(x) & 0xFu) << 24)
#define ADC_FCTRL_FWMARK(x)(((uint32_t)(x) & 0xFu) << 16)
#define ADC_CFG_PWRSEL(x)  (((uint32_t)(x) & 0x3u) << 4)
#define ADC_CFG_REFSEL(x)  (((uint32_t)(x) & 0x3u) << 6)
#define ADC_RESFIFO_D      (0xFFFFu)
#define ADC_RESFIFO_VALID  (1u << 31)

/* LPADC clock roots + LPCG gates */
#define CCM_CLOCK_ROOT9_CONTROL  (*(volatile uint32_t *)0x40CC0480u)
#define CCM_CLOCK_ROOT10_CONTROL (*(volatile uint32_t *)0x40CC0500u)
#define CCM_LPCG55_DIRECT (*(volatile uint32_t *)0x40CC66E0u)
#define CCM_LPCG56_DIRECT (*(volatile uint32_t *)0x40CC6700u)

/* IOMUXC_GPR FlexRAM bank-config registers (base 0x400E4000) */
#define IOMUXC_GPR_GPR14 (*(volatile uint32_t *)0x400E4038u)
#define IOMUXC_GPR_GPR16 (*(volatile uint32_t *)0x400E4040u)
#define IOMUXC_GPR_GPR17 (*(volatile uint32_t *)0x400E4044u)  /* FLEXRAM_BANK_CFG_LOW  (banks 0-7)  */
#define IOMUXC_GPR_GPR18 (*(volatile uint32_t *)0x400E4048u)  /* FLEXRAM_BANK_CFG_HIGH (banks 8-15) */

/* SRC (System Reset Controller, base 0x40C04000) - dual-core reset control.
 * SRSR bitmap (nxp PERI_SRC.h): bit2 M7_LOCKUP, bit12 M4_LOCKUP_M7, bit0/16 POR. */
#define SRC_SRMR (*(volatile uint32_t *)0x40C04004u)  /* [7:6] M4LOCKUP_RESET_MODE, [11:10] M4REQ_RESET_MODE */
#define SRC_SRSR (*(volatile uint32_t *)0x40C04010u)  /* reset status (write 1 to clear) */

/* --- Dual-core (CM7 boots CM4): SRC core-release + IOMUXC_LPSR_GPR CM4 VTOR
 *     + CM4 TCM backdoor + MU messaging unit.  RM ch.25 (SRC), ch.12
 *     (IOMUXC_LPSR_GPR), ch.35 (MU).  Every register/quirk below was
 *     HW-verified on the EVKB by evkb/dualcore_mu_test (2026-07-16).
 *     Consumed by Multicore.{h,cpp} + MessagingUnit.{h,cpp}. --- */
/* SRC dual-core reset/release (base 0x40C04000). */
#define SRC_SCR         (*(volatile uint32_t *)0x40C04000u)  /* boot-release control */
#define SRC_CTRL_M4CORE (*(volatile uint32_t *)0x40C04284u)  /* CM4 reset-slice control */
#define SRC_STAT_M4CORE (*(volatile uint32_t *)0x40C04290u)  /* CM4 reset-slice status */
#define SRC_SCR_BT_RELEASE_M4     (1u << 0)  /* write-1-ONLY: release CM4 from reset (a 0 write is ignored on silicon) */
#define SRC_SCR_BT_RELEASE_M7     (1u << 1)
#define SRC_CTRL_M4CORE_SW_RESET  (1u << 0)  /* self-clearing: SW-reset the CM4 slice (reboots a released CM4 from its VTOR) */
#define SRC_STAT_M4CORE_UNDER_RST (1u << 0)  /* 1 = CM4 held, 0 = released -- the RELIABLE hold indicator (MU ASR.RS bit7 never sets on silicon) */

/* IOMUXC_LPSR_GPR CM4 initial vector table (base 0x40C0C000):
 * GPR0[15:3] = CM4_INIT_VTOR[15:3], GPR1[15:0] = CM4_INIT_VTOR[31:16]. */
#define IOMUXC_LPSR_GPR0 (*(volatile uint32_t *)0x40C0C000u)
#define IOMUXC_LPSR_GPR1 (*(volatile uint32_t *)0x40C0C004u)

/* CM4 image staging window: the CM7 reaches the CM4 TCMs through this OCRAM
 * backdoor alias (RM Table 3-1: 0x20200000 -> CM4 ITCM, 0x20220000 -> CM4
 * DTCM).  Stage the image here AND boot from here (VTOR = 0x20200000); a
 * CM4-private TCM-window VTOR (0x1FFE0000) releases the slice but never
 * fetches (EVKB-verified). */
#define CM4_TCM_BACKDOOR   0x20200000u
#define CM4_BOOT_ADDRESS   CM4_TCM_BACKDOOR

/* CCM LPCG1 DIRECT: CM4 core clock gate (0x40CC6000 + 1*0x20).  The boot ROM
 * ungates it and the SDK/Zephyr never touch it; writing 1 is defensive
 * (HW-verified already-on: dualcore_mu_test lpcg=1). */
#define CCM_LPCG1_DIRECT (*(volatile uint32_t *)0x40CC6020u)

/* MU (Messaging Unit), RM ch.35.  This core is Processor A (MUA @ 0x40C48000);
 * the CM4 is Processor B (MUB @ 0x40C4C000).  Classic single-SR/CR layout:
 * TR[4]@0x00 (WO), RR[4]@0x10 (RO), SR@0x20 (GIPn is W1C), CR@0x24.  In every
 * 4-bit field n=0 is the HIGH-order bit (channel 0 -> +3, channel 3 -> +0). */
#define MUA_BASE 0x40C48000u
#define MUB_BASE 0x40C4C000u
#define MUA_TR(n) (*(volatile uint32_t *)(MUA_BASE + 0x00u + ((unsigned)(n) << 2)))
#define MUA_RR(n) (*(volatile uint32_t *)(MUA_BASE + 0x10u + ((unsigned)(n) << 2)))
#define MUA_SR    (*(volatile uint32_t *)(MUA_BASE + 0x20u))
#define MUA_CR    (*(volatile uint32_t *)(MUA_BASE + 0x24u))
#define MUB_SR    (*(volatile uint32_t *)(MUB_BASE + 0x20u))
/* SR fields (n = 0..3): */
#define MU_SR_GIP(n) (1u << (31 - (n)))   /* general-purpose interrupt pending (W1C) */
#define MU_SR_RF(n)  (1u << (27 - (n)))   /* receive-register n full */
#define MU_SR_TE(n)  (1u << (23 - (n)))   /* transmit-register n empty */
#define MU_SR_FUP    (1u << 8)
#define MU_SR_RS     (1u << 7)            /* RM "B in reset"; NEVER sets on RT1176 silicon -- do NOT use */
#define MU_SR_BIT9   (1u << 9)            /* undocumented; ALWAYS reads 1 on RT1176 silicon -- mask off before flag compares */
#define MU_SR_EP     (1u << 4)
#define MU_SR_Fn_MASK 0x7u
/* CR fields (n = 0..3): */
#define MU_CR_GIE(n) (1u << (31 - (n)))   /* general-purpose interrupt enable */
#define MU_CR_RIE(n) (1u << (27 - (n)))   /* receive interrupt enable */
#define MU_CR_TIE(n) (1u << (23 - (n)))   /* transmit interrupt enable */
#define MU_CR_GIR(n) (1u << (19 - (n)))   /* general-purpose interrupt request (auto-clears when peer W1C-acks its GIP) */
#define MU_CR_MUR    (1u << 5)
#define MU_CR_Fn_MASK 0x7u
#define MU_IRQ 118u   /* NVIC line for MU (ORed tx/rx/gp), both cores (RM interrupt table) */

/* LPI2C1 (Arduino-header I2C master), base 0x40104000 */
#define LPI2C1_MCR    (*(volatile uint32_t *)0x40104010u)
#define LPI2C1_MSR    (*(volatile uint32_t *)0x40104014u)
#define LPI2C1_MIER   (*(volatile uint32_t *)0x40104018u)
#define LPI2C1_MCFGR1 (*(volatile uint32_t *)0x40104024u)
#define LPI2C1_MCCR0  (*(volatile uint32_t *)0x40104048u)
#define LPI2C1_MFCR   (*(volatile uint32_t *)0x40104058u)
#define LPI2C1_MTDR   (*(volatile uint32_t *)0x40104060u)
#define LPI2C1_MRDR   (*(volatile uint32_t *)0x40104070u)
/* LPI2C1 clock: CCM_CLOCK_ROOT37 + LPCG98 */
#define CCM_CLOCK_ROOT37_CONTROL (*(volatile uint32_t *)0x40CC1280u)
#define CCM_LPCG98_DIRECT        (*(volatile uint32_t *)0x40CC6C40u)
/* LPI2C1 pins: SCL=GPIO_AD_08, SDA=GPIO_AD_09 (ALT1) */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_08 (*(volatile uint32_t *)0x400E812Cu)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_09 (*(volatile uint32_t *)0x400E8130u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_08 (*(volatile uint32_t *)0x400E8370u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_09 (*(volatile uint32_t *)0x400E8374u)
#define IOMUXC_LPI2C1_SCL_SELECT_INPUT   (*(volatile uint32_t *)0x400E85ACu)
#define IOMUXC_LPI2C1_SDA_SELECT_INPUT   (*(volatile uint32_t *)0x400E85B0u)

/* LPI2C1 slave register block */
#define LPI2C1_SCR    (*(volatile uint32_t *)0x40104110u)
#define LPI2C1_SSR    (*(volatile uint32_t *)0x40104114u)
#define LPI2C1_SIER   (*(volatile uint32_t *)0x40104118u)
#define LPI2C1_SCFGR1 (*(volatile uint32_t *)0x40104124u)
#define LPI2C1_SCFGR2 (*(volatile uint32_t *)0x40104128u)
#define LPI2C1_SAMR   (*(volatile uint32_t *)0x40104140u)
#define LPI2C1_SASR   (*(volatile uint32_t *)0x40104150u)
#define LPI2C1_STAR   (*(volatile uint32_t *)0x40104154u)
#define LPI2C1_STDR   (*(volatile uint32_t *)0x40104160u)
#define LPI2C1_SRDR   (*(volatile uint32_t *)0x40104170u)
/* LPI2C2 (QEMU loopback slave), base 0x40108000 */
#define LPI2C2_MCR    (*(volatile uint32_t *)0x40108010u)
#define LPI2C2_MSR    (*(volatile uint32_t *)0x40108014u)
#define LPI2C2_MIER   (*(volatile uint32_t *)0x40108018u)
#define LPI2C2_MCFGR1 (*(volatile uint32_t *)0x40108024u)
#define LPI2C2_MCCR0  (*(volatile uint32_t *)0x40108048u)
#define LPI2C2_MTDR   (*(volatile uint32_t *)0x40108060u)
#define LPI2C2_MRDR   (*(volatile uint32_t *)0x40108070u)
#define LPI2C2_SCR    (*(volatile uint32_t *)0x40108110u)
#define LPI2C2_SSR    (*(volatile uint32_t *)0x40108114u)
#define LPI2C2_SIER   (*(volatile uint32_t *)0x40108118u)
#define LPI2C2_SCFGR1 (*(volatile uint32_t *)0x40108124u)
#define LPI2C2_SCFGR2 (*(volatile uint32_t *)0x40108128u)
#define LPI2C2_SAMR   (*(volatile uint32_t *)0x40108140u)
#define LPI2C2_SASR   (*(volatile uint32_t *)0x40108150u)
#define LPI2C2_STAR   (*(volatile uint32_t *)0x40108154u)
#define LPI2C2_STDR   (*(volatile uint32_t *)0x40108160u)
#define LPI2C2_SRDR   (*(volatile uint32_t *)0x40108170u)
/* LPI2C2 clock: CCM_CLOCK_ROOT38 + LPCG99 */
#define CCM_CLOCK_ROOT38_CONTROL (*(volatile uint32_t *)0x40CC1300u)
#define CCM_LPCG99_DIRECT        (*(volatile uint32_t *)0x40CC6C60u)

/* LPSPI1 (Arduino-header SPI master), base 0x40114000 */
#define LPSPI1_CR     (*(volatile uint32_t *)0x40114010u)
#define LPSPI1_SR     (*(volatile uint32_t *)0x40114014u)
#define LPSPI1_DER    (*(volatile uint32_t *)0x4011401Cu)
#define LPSPI1_CFGR1  (*(volatile uint32_t *)0x40114024u)
#define LPSPI1_CCR    (*(volatile uint32_t *)0x40114040u)
#define LPSPI1_FCR    (*(volatile uint32_t *)0x40114058u)
#define LPSPI1_TCR    (*(volatile uint32_t *)0x40114060u)
#define LPSPI1_TDR    (*(volatile uint32_t *)0x40114064u)
#define LPSPI1_RSR    (*(volatile uint32_t *)0x40114070u)
#define LPSPI1_RDR    (*(volatile uint32_t *)0x40114074u)
/* LPSPI1 clock: CCM_CLOCK_ROOT43 + LPCG104 (mux 0 = 24 MHz) */
#define CCM_CLOCK_ROOT43_CONTROL (*(volatile uint32_t *)0x40CC1580u)
#define CCM_LPCG104_DIRECT       (*(volatile uint32_t *)0x40CC6D00u)
/* LPSPI1 pins: SCK=GPIO_AD_28, SOUT=GPIO_AD_30, SIN=GPIO_AD_31 (ALT0) */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_28 (*(volatile uint32_t *)0x400E817Cu)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_30 (*(volatile uint32_t *)0x400E8184u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_31 (*(volatile uint32_t *)0x400E8188u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_28 (*(volatile uint32_t *)0x400E83C0u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_30 (*(volatile uint32_t *)0x400E83C8u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_31 (*(volatile uint32_t *)0x400E83CCu)
#define IOMUXC_LPSPI1_SCK_SELECT_INPUT   (*(volatile uint32_t *)0x400E85D0u)
#define IOMUXC_LPSPI1_SDO_SELECT_INPUT   (*(volatile uint32_t *)0x400E85D8u)
#define IOMUXC_LPSPI1_SDI_SELECT_INPUT   (*(volatile uint32_t *)0x400E85D4u)

/* LPSPI register-block overlay (for the newdigate/SPI library's port()
 * accessor).  Layout matches cores/teensy4/imxrt.h IMXRT_LPSPI_t; verified
 * against the flat LPSPI1_* offsets above (base 0x40114000). */
typedef struct {
	volatile uint32_t VERID;    // 0x00
	volatile uint32_t PARAM;    // 0x04
	volatile uint32_t unused08; // 0x08
	volatile uint32_t unused0C; // 0x0C
	volatile uint32_t CR;       // 0x10
	volatile uint32_t SR;       // 0x14
	volatile uint32_t IER;      // 0x18
	volatile uint32_t DER;      // 0x1C
	volatile uint32_t CFGR0;    // 0x20
	volatile uint32_t CFGR1;    // 0x24
	volatile uint32_t unused28; // 0x28
	volatile uint32_t unused2C; // 0x2C
	volatile uint32_t DMR0;     // 0x30
	volatile uint32_t DMR1;     // 0x34
	volatile uint32_t unused38; // 0x38
	volatile uint32_t unused3C; // 0x3C
	volatile uint32_t CCR;      // 0x40
	volatile uint32_t unused44[5]; // 0x44..0x54
	volatile uint32_t FCR;      // 0x58
	volatile uint32_t FSR;      // 0x5C
	volatile uint32_t TCR;      // 0x60
	volatile uint32_t TDR;      // 0x64
	volatile uint32_t unused68; // 0x68
	volatile uint32_t unused6C; // 0x6C
	volatile uint32_t RSR;      // 0x70
	volatile uint32_t RDR;      // 0x74
} IMXRT_LPSPI_t;
#define IMXRT_LPSPI1_ADDRESS 0x40114000

/* LPI2C register-block overlay (for the newdigate/Wire library's port()
 * accessor).  Layout matches cores/teensy4/imxrt.h IMXRT_LPI2C_t; verified
 * against the flat LPI2C1_* offsets above.  Master block 0x10..0x70, then a
 * gap, slave block 0x110..0x170.  Same layout at all 3 bases (LPI2C1
 * 0x40104000, LPI2C2 0x40108000, LPI2C5 0x40C34000). */
typedef struct {
	volatile uint32_t VERID;        // 0x00
	volatile uint32_t PARAM;        // 0x04
	volatile uint32_t unused08;     // 0x08
	volatile uint32_t unused0C;     // 0x0C
	volatile uint32_t MCR;          // 0x10
	volatile uint32_t MSR;          // 0x14
	volatile uint32_t MIER;         // 0x18
	volatile uint32_t MDER;         // 0x1C
	volatile uint32_t MCFGR0;       // 0x20
	volatile uint32_t MCFGR1;       // 0x24
	volatile uint32_t MCFGR2;       // 0x28
	volatile uint32_t MCFGR3;       // 0x2C
	volatile uint32_t unused30[4];  // 0x30..0x3C
	volatile uint32_t MDMR;         // 0x40
	volatile uint32_t unused44;     // 0x44
	volatile uint32_t MCCR0;        // 0x48
	volatile uint32_t unused4C;     // 0x4C
	volatile uint32_t MCCR1;        // 0x50
	volatile uint32_t unused54;     // 0x54
	volatile uint32_t MFCR;         // 0x58
	volatile uint32_t MFSR;         // 0x5C
	volatile uint32_t MTDR;         // 0x60
	volatile uint32_t unused64[3];  // 0x64..0x6C
	volatile uint32_t MRDR;         // 0x70
	volatile uint32_t unused74[39]; // 0x74..0x10C
	volatile uint32_t SCR;          // 0x110
	volatile uint32_t SSR;          // 0x114
	volatile uint32_t SIER;         // 0x118
	volatile uint32_t SDER;         // 0x11C
	volatile uint32_t unused120;    // 0x120
	volatile uint32_t SCFGR1;       // 0x124
	volatile uint32_t SCFGR2;       // 0x128
	volatile uint32_t unused12C[5]; // 0x12C..0x13C
	volatile uint32_t SAMR;         // 0x140
	volatile uint32_t unused144[3]; // 0x144..0x14C
	volatile uint32_t SASR;         // 0x150
	volatile uint32_t STAR;         // 0x154
	volatile uint32_t unused158[2]; // 0x158..0x15C
	volatile uint32_t STDR;         // 0x160
	volatile uint32_t unused164[3]; // 0x164..0x16C
	volatile uint32_t SRDR;         // 0x170
} IMXRT_LPI2C_t;
#define IMXRT_LPI2C1_ADDRESS 0x40104000
#define IMXRT_LPI2C2_ADDRESS 0x40108000
#define IMXRT_LPI2C5_ADDRESS 0x40C34000

/* USDHC1 microSD pins (EVKB J15): CMD/CLK/DATA0-3 = GPIO_SD_B1_00..05, ALT0 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B1_00 (*(volatile uint32_t *)0x400E819Cu)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B1_01 (*(volatile uint32_t *)0x400E81A0u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B1_02 (*(volatile uint32_t *)0x400E81A4u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B1_03 (*(volatile uint32_t *)0x400E81A8u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B1_04 (*(volatile uint32_t *)0x400E81ACu)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B1_05 (*(volatile uint32_t *)0x400E81B0u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_SD_B1_00 (*(volatile uint32_t *)0x400E83E0u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_SD_B1_01 (*(volatile uint32_t *)0x400E83E4u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_SD_B1_02 (*(volatile uint32_t *)0x400E83E8u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_SD_B1_03 (*(volatile uint32_t *)0x400E83ECu)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_SD_B1_04 (*(volatile uint32_t *)0x400E83F0u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_SD_B1_05 (*(volatile uint32_t *)0x400E83F4u)
/* USDHC1 clock root 58 (SYS_PLL2_PFD2 / 2) + gate LPCG117 */
#define CCM_CLOCK_ROOT58_CONTROL (*(volatile uint32_t *)0x40CC1D00u)
#define CCM_LPCG117_DIRECT       (*(volatile uint32_t *)0x40CC6EA0u)

/* USDHC register-block overlay (for the SdFat SdioTeensy __IMXRT1176__ port).
 * Layout matches cores/teensy4/imxrt.h IMXRT_USDHC_t; only the base addresses
 * differ from the 1062 (0x40418000/0x4041C000 vs 0x402C0000/0x402C4000). */
typedef struct {
	volatile uint32_t DS_ADDR;              // 0x00
	volatile uint32_t BLK_ATT;              // 0x04
	volatile uint32_t CMD_ARG;              // 0x08
	volatile uint32_t CMD_XFR_TYP;          // 0x0C
	volatile uint32_t CMD_RSP0;             // 0x10
	volatile uint32_t CMD_RSP1;             // 0x14
	volatile uint32_t CMD_RSP2;             // 0x18
	volatile uint32_t CMD_RSP3;             // 0x1C
	volatile uint32_t DATA_BUFF_ACC_PORT;   // 0x20
	volatile uint32_t PRES_STATE;           // 0x24
	volatile uint32_t PROT_CTRL;            // 0x28
	volatile uint32_t SYS_CTRL;             // 0x2C
	volatile uint32_t INT_STATUS;           // 0x30
	volatile uint32_t INT_STATUS_EN;        // 0x34
	volatile uint32_t INT_SIGNAL_EN;        // 0x38
	volatile uint32_t AUTOCMD12_ERR_STATUS; // 0x3C
	volatile uint32_t HOST_CTRL_CAP;        // 0x40
	volatile uint32_t WTMK_LVL;             // 0x44
	volatile uint32_t MIX_CTRL;             // 0x48
	uint32_t unused1;                       // 0x4C
	volatile uint32_t FORCE_EVENT;          // 0x50
	volatile uint32_t ADMA_ERR_STATUS;      // 0x54
	volatile uint32_t ADMA_SYS_ADDR;        // 0x58
	uint32_t unused2;                       // 0x5C
	volatile uint32_t DLL_CTRL;             // 0x60
	volatile uint32_t DLL_STATUS;           // 0x64
	volatile uint32_t CLK_TUNE_CTRL_STATUS; // 0x68
	uint32_t unused3[21];                   // 0x6C..0xBC
	volatile uint32_t VEND_SPEC;            // 0xC0
	volatile uint32_t MMC_BOOT;             // 0xC4
	volatile uint32_t VEND_SPEC2;           // 0xC8
	volatile uint32_t TUNING_CTRL;          // 0xCC
} IMXRT_USDHC_t;
#define IMXRT_USDHC1_ADDRESS 0x40418000
#define IMXRT_USDHC2_ADDRESS 0x4041C000
#define IMXRT_USDHC1 (*(IMXRT_USDHC_t *)IMXRT_USDHC1_ADDRESS)
#define IMXRT_USDHC2 (*(IMXRT_USDHC_t *)IMXRT_USDHC2_ADDRESS)
#define USDHC1_DS_ADDR (IMXRT_USDHC1.DS_ADDR)
#define USDHC1_BLK_ATT (IMXRT_USDHC1.BLK_ATT)
#define USDHC1_CMD_ARG (IMXRT_USDHC1.CMD_ARG)
#define USDHC1_CMD_XFR_TYP (IMXRT_USDHC1.CMD_XFR_TYP)
#define USDHC1_CMD_RSP0 (IMXRT_USDHC1.CMD_RSP0)
#define USDHC1_CMD_RSP1 (IMXRT_USDHC1.CMD_RSP1)
#define USDHC1_CMD_RSP2 (IMXRT_USDHC1.CMD_RSP2)
#define USDHC1_CMD_RSP3 (IMXRT_USDHC1.CMD_RSP3)
#define USDHC1_DATA_BUFF_ACC_PORT (IMXRT_USDHC1.DATA_BUFF_ACC_PORT)
#define USDHC1_PRES_STATE (IMXRT_USDHC1.PRES_STATE)
#define USDHC1_PROT_CTRL (IMXRT_USDHC1.PROT_CTRL)
#define USDHC1_SYS_CTRL (IMXRT_USDHC1.SYS_CTRL)
#define USDHC1_INT_STATUS (IMXRT_USDHC1.INT_STATUS)
#define USDHC1_INT_STATUS_EN (IMXRT_USDHC1.INT_STATUS_EN)
#define USDHC1_INT_SIGNAL_EN (IMXRT_USDHC1.INT_SIGNAL_EN)
#define USDHC1_AUTOCMD12_ERR_STATUS (IMXRT_USDHC1.AUTOCMD12_ERR_STATUS)
#define USDHC1_HOST_CTRL_CAP (IMXRT_USDHC1.HOST_CTRL_CAP)
#define USDHC1_WTMK_LVL (IMXRT_USDHC1.WTMK_LVL)
#define USDHC1_MIX_CTRL (IMXRT_USDHC1.MIX_CTRL)
#define USDHC1_FORCE_EVENT (IMXRT_USDHC1.FORCE_EVENT)
#define USDHC1_ADMA_ERR_STATUS (IMXRT_USDHC1.ADMA_ERR_STATUS)
#define USDHC1_ADMA_SYS_ADDR (IMXRT_USDHC1.ADMA_SYS_ADDR)
#define USDHC1_DLL_CTRL (IMXRT_USDHC1.DLL_CTRL)
#define USDHC1_DLL_STATUS (IMXRT_USDHC1.DLL_STATUS)
#define USDHC1_CLK_TUNE_CTRL_STATUS (IMXRT_USDHC1.CLK_TUNE_CTRL_STATUS)
#define USDHC1_VEND_SPEC (IMXRT_USDHC1.VEND_SPEC)
#define USDHC1_MMC_BOOT (IMXRT_USDHC1.MMC_BOOT)
#define USDHC1_VEND_SPEC2 (IMXRT_USDHC1.VEND_SPEC2)
#define USDHC1_TUNING_CTRL (IMXRT_USDHC1.TUNING_CTRL)
#define USDHC2_DS_ADDR (IMXRT_USDHC2.DS_ADDR)
#define USDHC2_BLK_ATT (IMXRT_USDHC2.BLK_ATT)
#define USDHC2_CMD_ARG (IMXRT_USDHC2.CMD_ARG)
#define USDHC2_CMD_XFR_TYP (IMXRT_USDHC2.CMD_XFR_TYP)
#define USDHC2_CMD_RSP0 (IMXRT_USDHC2.CMD_RSP0)
#define USDHC2_CMD_RSP1 (IMXRT_USDHC2.CMD_RSP1)
#define USDHC2_CMD_RSP2 (IMXRT_USDHC2.CMD_RSP2)
#define USDHC2_CMD_RSP3 (IMXRT_USDHC2.CMD_RSP3)
#define USDHC2_DATA_BUFF_ACC_PORT (IMXRT_USDHC2.DATA_BUFF_ACC_PORT)
#define USDHC2_PRES_STATE (IMXRT_USDHC2.PRES_STATE)
#define USDHC2_PROT_CTRL (IMXRT_USDHC2.PROT_CTRL)
#define USDHC2_SYS_CTRL (IMXRT_USDHC2.SYS_CTRL)
#define USDHC2_INT_STATUS (IMXRT_USDHC2.INT_STATUS)
#define USDHC2_INT_STATUS_EN (IMXRT_USDHC2.INT_STATUS_EN)
#define USDHC2_INT_SIGNAL_EN (IMXRT_USDHC2.INT_SIGNAL_EN)
#define USDHC2_AUTOCMD12_ERR_STATUS (IMXRT_USDHC2.AUTOCMD12_ERR_STATUS)
#define USDHC2_HOST_CTRL_CAP (IMXRT_USDHC2.HOST_CTRL_CAP)
#define USDHC2_WTMK_LVL (IMXRT_USDHC2.WTMK_LVL)
#define USDHC2_MIX_CTRL (IMXRT_USDHC2.MIX_CTRL)
#define USDHC2_FORCE_EVENT (IMXRT_USDHC2.FORCE_EVENT)
#define USDHC2_ADMA_ERR_STATUS (IMXRT_USDHC2.ADMA_ERR_STATUS)
#define USDHC2_ADMA_SYS_ADDR (IMXRT_USDHC2.ADMA_SYS_ADDR)
#define USDHC2_DLL_CTRL (IMXRT_USDHC2.DLL_CTRL)
#define USDHC2_DLL_STATUS (IMXRT_USDHC2.DLL_STATUS)
#define USDHC2_CLK_TUNE_CTRL_STATUS (IMXRT_USDHC2.CLK_TUNE_CTRL_STATUS)
#define USDHC2_VEND_SPEC (IMXRT_USDHC2.VEND_SPEC)
#define USDHC2_MMC_BOOT (IMXRT_USDHC2.MMC_BOOT)
#define USDHC2_VEND_SPEC2 (IMXRT_USDHC2.VEND_SPEC2)
#define USDHC2_TUNING_CTRL (IMXRT_USDHC2.TUNING_CTRL)

/* Digital header GPIO bases (GPIO8/9/11/12_BASE already defined above) */
/* Arduino-header pin pads (GPIO ALT = 0xA) */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_10 (*(volatile uint32_t *)0x400E823Cu)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_11 (*(volatile uint32_t *)0x400E8240u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_12 (*(volatile uint32_t *)0x400E8244u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_10 (*(volatile uint32_t *)0x400E8480u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_11 (*(volatile uint32_t *)0x400E8484u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_12 (*(volatile uint32_t *)0x400E8488u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_LPSR_04 (*(volatile uint32_t *)0x40C08010u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_LPSR_05 (*(volatile uint32_t *)0x40C08014u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_LPSR_04 (*(volatile uint32_t *)0x40C08050u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_LPSR_05 (*(volatile uint32_t *)0x40C08054u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_00 (*(volatile uint32_t *)0x400E810Cu)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_01 (*(volatile uint32_t *)0x400E8110u)
/* IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_04 already defined above */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_05 (*(volatile uint32_t *)0x400E8120u)
/* IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_06 already defined above */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_07 (*(volatile uint32_t *)0x400E8128u)
/* IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_08/09 already defined above (LPI2C1 block) */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_10 (*(volatile uint32_t *)0x400E8134u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_11 (*(volatile uint32_t *)0x400E8138u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_12 (*(volatile uint32_t *)0x400E813Cu)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_13 (*(volatile uint32_t *)0x400E8140u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_14 (*(volatile uint32_t *)0x400E8144u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_29 (*(volatile uint32_t *)0x400E8180u)
/* IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_28/30/31 already defined above (LPSPI1 block) */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_00 (*(volatile uint32_t *)0x400E8350u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_01 (*(volatile uint32_t *)0x400E8354u)
/* IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_04 already defined above */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_05 (*(volatile uint32_t *)0x400E8364u)
/* IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_06 already defined above */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_07 (*(volatile uint32_t *)0x400E836Cu)
/* IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_08/09 already defined above (LPI2C1 block) */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_10 (*(volatile uint32_t *)0x400E8378u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_11 (*(volatile uint32_t *)0x400E837Cu)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_12 (*(volatile uint32_t *)0x400E8380u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_13 (*(volatile uint32_t *)0x400E8384u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_14 (*(volatile uint32_t *)0x400E8388u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_29 (*(volatile uint32_t *)0x400E83C4u)
/* IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_28/30/31 already defined above (LPSPI1 block) */
/* FlexPWM (analogWrite): module bases + per-module clock gates */
#define FLEXPWM1_BASE 0x4018C000u
#define FLEXPWM2_BASE 0x40190000u
#define FLEXPWM3_BASE 0x40194000u
#define FLEXPWM4_BASE 0x40198000u
#define CCM_LPCG79_DIRECT (*(volatile uint32_t *)0x40CC69E0u)  /* FlexPWM1 */
#define CCM_LPCG81_DIRECT (*(volatile uint32_t *)0x40CC6A20u)  /* FlexPWM3 */

/* ---- PIT1 (Periodic Interrupt Timer): 4 channels, combined IRQ 155 ------ */
#define PIT1_BASE          0x400D8000u
#define PIT1_MCR           (*(volatile uint32_t *)(PIT1_BASE + 0x00u))
#define PIT1_MCR_MDIS      (1u << 1)         /* module disable (clear to enable) */
typedef struct {
    volatile uint32_t LDVAL;    /* +0x0 load value                */
    volatile uint32_t CVAL;     /* +0x4 current value (read-only) */
    volatile uint32_t TCTRL;    /* +0x8 timer control             */
    volatile uint32_t TFLG;     /* +0xC timer flag (W1C)          */
} pit_channel_t;
#define PIT1_CHANNEL       ((pit_channel_t *)(PIT1_BASE + 0x100u))
#define PIT_TCTRL_TEN      (1u << 0)         /* timer enable           */
#define PIT_TCTRL_TIE      (1u << 1)         /* timer interrupt enable */
#define PIT_TFLG_TIF       (1u << 0)         /* timeout flag (W1C)     */
/* PIT1 clock gate: CCM LPCG62 DIRECT (kCLOCK_Pit1=62 -> 0x6000 + 62*0x20). */
#define CCM_LPCG62_DIRECT  (*(volatile uint32_t *)0x40CC67C0u)

/* ---- USB_OTG1 (ChipIdea/EHCI device controller) + USBPHY1 -------------------
 * Same IP as imxrt1062, so cores/teensy4/usb.c ports verbatim; only the base
 * changes (1062 0x402E0000 -> 0x40430000). Device register window at base+0x140.
 * Register/bit names match Teensy so the port needs no renaming. IRQ = 136. */
#define USB_OTG1_BASE          0x40430000u
#define USB1_USBCMD            (*(volatile uint32_t *)(USB_OTG1_BASE + 0x140))
#define USB1_USBSTS            (*(volatile uint32_t *)(USB_OTG1_BASE + 0x144))
#define USB1_USBINTR           (*(volatile uint32_t *)(USB_OTG1_BASE + 0x148))
#define USB1_DEVICEADDR        (*(volatile uint32_t *)(USB_OTG1_BASE + 0x154))
#define USB1_ENDPOINTLISTADDR  (*(volatile uint32_t *)(USB_OTG1_BASE + 0x158))
#define USB1_BURSTSIZE         (*(volatile uint32_t *)(USB_OTG1_BASE + 0x160))
#define USB1_PORTSC1           (*(volatile uint32_t *)(USB_OTG1_BASE + 0x184))
#define USB1_USBMODE           (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1A8))
#define USB1_ENDPTSETUPSTAT    (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1AC))
#define USB1_ENDPTPRIME        (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1B0))
#define USB1_ENDPTFLUSH        (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1B4))
#define USB1_ENDPTSTATUS       (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1B8))
#define USB1_ENDPTCOMPLETE     (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1BC))
#define USB1_ENDPTCTRL0        (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1C0))
#define USB1_ENDPTCTRL1        (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1C4))
#define USB1_ENDPTCTRL2        (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1C8))
#define USB1_ENDPTCTRL3        (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1CC))
#define USB1_ENDPTCTRL4        (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1D0))
#define USB1_ENDPTCTRL5        (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1D4))
#define USB1_ENDPTCTRL6        (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1D8))
#define USB1_ENDPTCTRL7        (*(volatile uint32_t *)(USB_OTG1_BASE + 0x1DC))

/* USB GPTIMER0 — general-purpose one-shot; usb_serial TX auto-flush (Phase 2). */
#define USB1_GPTIMER0LD        (*(volatile uint32_t *)(USB_OTG1_BASE + 0x080))
#define USB1_GPTIMER0CTRL      (*(volatile uint32_t *)(USB_OTG1_BASE + 0x084))
/* USB_USBSTS_TI0 already defined below (USBSTS/USBINTR bit masks block). */
#define USB_USBINTR_TIE0          ((uint32_t)(1u<<24))
#define USB_GPTIMERCTRL_GPTRUN    ((uint32_t)(1u<<31))
#define USB_GPTIMERCTRL_GPTRST    ((uint32_t)(1u<<30))

/* D-cache is OFF in this core (Phase-1 HW-confirmed) → cache maintenance is a
 * no-op.  Provided so the teensy4 usb_serial.c port compiles verbatim.  If a
 * future build enables the D-cache, replace these with real CMSIS operations. */
static inline void arm_dcache_delete(void *addr, uint32_t size) { (void)addr; (void)size; }
static inline void arm_dcache_flush_delete(void *addr, uint32_t size) { (void)addr; (void)size; }

#define USBPHY1_BASE           0x40434000u
#define USBPHY1_PWD            (*(volatile uint32_t *)(USBPHY1_BASE + 0x00))
#define USBPHY1_CTRL           (*(volatile uint32_t *)(USBPHY1_BASE + 0x30))
#define USBPHY1_CTRL_SET       (*(volatile uint32_t *)(USBPHY1_BASE + 0x34))
#define USBPHY1_CTRL_CLR       (*(volatile uint32_t *)(USBPHY1_BASE + 0x38))
#define USBPHY1_PLL_SIC        (*(volatile uint32_t *)(USBPHY1_BASE + 0xA0))
#define USBPHY1_PLL_SIC_SET    (*(volatile uint32_t *)(USBPHY1_BASE + 0xA4))
#define USBPHY1_PLL_SIC_CLR    (*(volatile uint32_t *)(USBPHY1_BASE + 0xA8))
#define CCM_LPCG115_DIRECT     (*(volatile uint32_t *)0x40CC6E60u)   /* USB (kCLOCK_Usb=115) */
/* USBCMD / USBSTS / USBINTR / USBMODE / DEVICEADDR / ENDPTCTRL bit masks */
#define USB_USBCMD_RST         (1u << 1)
#define USB_USBCMD_RS          (1u << 0)
#define USB_USBCMD_SUTW        (1u << 13)
#define USB_USBCMD_ATDTW       (1u << 14)
#define USB_USBSTS_TI1         (1u << 25)
#define USB_USBSTS_TI0         (1u << 24)
#define USB_USBSTS_SLI         (1u << 8)
#define USB_USBSTS_SRI         (1u << 7)
#define USB_USBSTS_URI         (1u << 6)
#define USB_USBSTS_PCI         (1u << 2)
#define USB_USBSTS_UEI         (1u << 1)
#define USB_USBSTS_UI          (1u << 0)
#define USB_USBINTR_SLE        (1u << 8)
#define USB_USBINTR_SRE        (1u << 7)
#define USB_USBINTR_URE        (1u << 6)
#define USB_USBINTR_PCE        (1u << 2)
#define USB_USBINTR_UEE        (1u << 1)
#define USB_USBINTR_UE         (1u << 0)
#define USB_USBMODE_SLOM       (1u << 3)
#define USB_USBMODE_CM(n)      (((n) & 0x03u) << 0)
#define USB_USBMODE_CM_MASK    (0x03u)
#define USB_DEVICEADDR_USBADR(n) (((n) & 0x7Fu) << 25)
#define USB_DEVICEADDR_USBADRA (1u << 24)
#define USB_ENDPTCTRL_TXS      (1u << 16)
#define USB_ENDPTCTRL_RXS      (1u << 0)
#define USB_PORTSC1_HSP        (1u << 9)
#define USB_PORTSC1_PR         (1u << 8)
/* USBPHY CTRL / PWD / PLL_SIC bit masks (PLL_SIC values from SDK PERI_USBPHY.h) */
#define USBPHY_CTRL_SFTRST     (1u << 31)
#define USBPHY_CTRL_CLKGATE    (1u << 30)
#define USBPHY_CTRL_ENHOSTDISCONDETECT (1u << 1)  /* host: HS disconnect detector (USBHost_t36 ehci isr) */
#define USBPHY_PWD_RXPWDRX     (1u << 20)
#define USBPHY_PWD_RXPWDDIFF   (1u << 19)
#define USBPHY_PWD_RXPWD1PT1   (1u << 18)
#define USBPHY_PWD_RXPWDENV    (1u << 17)
#define USBPHY_PWD_TXPWDV2I    (1u << 12)
#define USBPHY_PWD_TXPWDIBIAS  (1u << 11)
#define USBPHY_PWD_TXPWDFS     (1u << 10)
#define USBPHY_PLL_SIC_PLL_EN_USB_CLKS  (0x40u)
#define USBPHY_PLL_SIC_PLL_POWER        (0x1000u)
#define USBPHY_PLL_SIC_PLL_BYPASS       (0x10000u)
#define USBPHY_PLL_SIC_PLL_REG_ENABLE   (0x200000u)
#define USBPHY_PLL_SIC_PLL_DIV_SEL(n)   (((n) & 0x7u) << 22)
#define USBPHY_PLL_SIC_PLL_DIV_SEL_MASK (0x1C00000u)
#define USBPHY_PLL_SIC_PLL_LOCK         (0x80000000u)

/* DMA structures (USB dQH/dTD/buffers) -> .dmabuffers (OCRAM). */
#ifndef DMAMEM
#define DMAMEM __attribute__((section(".dmabuffers"), used))
#endif

/* ---- USB_OTG2 (ChipIdea/EHCI HOST controller) + USBPHY2 --------------------
 * Host-role twin of USB_OTG1 above, added for the USBHost_t36 port.  That
 * library's utility/imxrt_usbhs.h maps USBHS_* -> USB2_* verbatim, exactly as
 * on imxrt1062 where USB2/USBPHY2 are likewise the host controller/PHY.  Same
 * ChipIdea IP and register offsets as USB1; only the bases differ (OTG
 * 0x40430000 -> 0x4042C000, PHY 0x40434000 -> 0x40438000).  The host-mode
 * register names FRINDEX / PERIODICLISTBASE / ASYNCLISTADDR alias the very
 * offsets the device block calls DEVICEADDR / ENDPOINTLISTADDR.  IRQ = 135,
 * declared as IRQ_USB_OTG2 in core_pins.h (with an IRQ_USB2 alias there so
 * imxrt_usbhs.h's IRQ_USBHS resolves).  Bases + IRQ verified vs the qemu2
 * fsl-imxrt1170 model; offsets/bits vs CMSIS PERI_USB.h / PERI_USBPHY.h. */
#define USB_OTG2_BASE          0x4042C000u
#define USB2_GPTIMER0LD        (*(volatile uint32_t *)(USB_OTG2_BASE + 0x080))
#define USB2_GPTIMER0CTRL      (*(volatile uint32_t *)(USB_OTG2_BASE + 0x084))
#define USB2_GPTIMER1LD        (*(volatile uint32_t *)(USB_OTG2_BASE + 0x088))
#define USB2_GPTIMER1CTRL      (*(volatile uint32_t *)(USB_OTG2_BASE + 0x08C))
#define USB2_SBUSCFG           (*(volatile uint32_t *)(USB_OTG2_BASE + 0x090))
#define USB2_USBCMD            (*(volatile uint32_t *)(USB_OTG2_BASE + 0x140))
#define USB2_USBSTS            (*(volatile uint32_t *)(USB_OTG2_BASE + 0x144))
#define USB2_USBINTR           (*(volatile uint32_t *)(USB_OTG2_BASE + 0x148))
#define USB2_FRINDEX           (*(volatile uint32_t *)(USB_OTG2_BASE + 0x14C))
#define USB2_PERIODICLISTBASE  (*(volatile uint32_t *)(USB_OTG2_BASE + 0x154))
#define USB2_ASYNCLISTADDR     (*(volatile uint32_t *)(USB_OTG2_BASE + 0x158))
#define USB2_BURSTSIZE         (*(volatile uint32_t *)(USB_OTG2_BASE + 0x160))
#define USB2_PORTSC1           (*(volatile uint32_t *)(USB_OTG2_BASE + 0x184))
#define USB2_USBMODE           (*(volatile uint32_t *)(USB_OTG2_BASE + 0x1A8))

#define USBPHY2_BASE           0x40438000u
#define USBPHY2_PWD            (*(volatile uint32_t *)(USBPHY2_BASE + 0x00))
#define USBPHY2_CTRL           (*(volatile uint32_t *)(USBPHY2_BASE + 0x30))
#define USBPHY2_CTRL_SET       (*(volatile uint32_t *)(USBPHY2_BASE + 0x34))
#define USBPHY2_CTRL_CLR       (*(volatile uint32_t *)(USBPHY2_BASE + 0x38))
#define USBPHY2_PLL_SIC        (*(volatile uint32_t *)(USBPHY2_BASE + 0xA0))
#define USBPHY2_PLL_SIC_SET    (*(volatile uint32_t *)(USBPHY2_BASE + 0xA4))
#define USBPHY2_PLL_SIC_CLR    (*(volatile uint32_t *)(USBPHY2_BASE + 0xA8))

/* Extra generic USB_* bit masks referenced by imxrt_usbhs.h's host block but
 * not used by the device (USB1) side, hence absent from the device masks
 * above.  Standard EHCI / ChipIdea positions, each cross-checked against the
 * CMSIS PERI_USB.h *_MASK defines.  (RS / RST / PCE / UEE / TI0 / TI1 / PR /
 * HSP / USBMODE_CM and the GPTIMER RST/RUN masks already exist with the device
 * block; USBSTS UAI/UPI are inlined literally in imxrt_usbhs.h, not here.) */
/* USBCMD */
#define USB_USBCMD_FS_1(n)     (((n) & 0x3u) << 2)
#define USB_USBCMD_PSE         (1u << 4)
#define USB_USBCMD_ASE         (1u << 5)
#define USB_USBCMD_IAA         (1u << 6)
#define USB_USBCMD_ASP(n)      (((n) & 0x3u) << 8)
#define USB_USBCMD_ASPE        (1u << 11)
#define USB_USBCMD_FS_2        (1u << 15)
#define USB_USBCMD_ITC(n)      (((n) & 0xFFu) << 16)
/* USBSTS */
#define USB_USBSTS_SEI         (1u << 4)
#define USB_USBSTS_AAI         (1u << 5)
#define USB_USBSTS_HCH         (1u << 12)
#define USB_USBSTS_AS          (1u << 15)
#define USB_USBSTS_NAKI        (1u << 16)
/* USBINTR */
#define USB_USBINTR_SEE        (1u << 4)
#define USB_USBINTR_UAIE       (1u << 18)
#define USB_USBINTR_UPIE       (1u << 19)
#define USB_USBINTR_TIE1       (1u << 25)
/* PORTSC1 */
#define USB_PORTSC1_CCS        (1u << 0)
#define USB_PORTSC1_CSC        (1u << 1)
#define USB_PORTSC1_PE         (1u << 2)
#define USB_PORTSC1_PEC        (1u << 3)
#define USB_PORTSC1_OCC        (1u << 5)
#define USB_PORTSC1_FPR        (1u << 6)
#define USB_PORTSC1_PP         (1u << 12)
#define USB_PORTSC1_PFSC       (1u << 24)

/* --- SAI1 (I2S TX, Arduino audio) base 0x40404000, IRQ 76 --- */
#define SAI1_TCSR (*(volatile uint32_t *)0x40404008u)
#define SAI1_TCR1 (*(volatile uint32_t *)0x4040400Cu)
#define SAI1_TCR2 (*(volatile uint32_t *)0x40404010u)
#define SAI1_TCR3 (*(volatile uint32_t *)0x40404014u)
#define SAI1_TCR4 (*(volatile uint32_t *)0x40404018u)
#define SAI1_TCR5 (*(volatile uint32_t *)0x4040401Cu)
#define SAI1_TDR0 (*(volatile uint32_t *)0x40404020u)
#define SAI1_TFR0 (*(volatile uint32_t *)0x40404040u)
#define SAI1_TMR  (*(volatile uint32_t *)0x40404060u)
#define SAI_TCSR_TE  (1u << 31)
#define SAI_TCSR_BCE (1u << 28)
#define SAI_TCSR_FR  (1u << 25)
#define SAI_TCSR_SR  (1u << 24)
#define SAI_TCSR_FEF (1u << 18)
#define SAI_TCSR_FWF (1u << 17)
#define SAI_TCSR_FRF (1u << 16)
#define SAI_TCSR_FRDE (1u << 0)
#define SAI_TCR2_DIV(x)  ((uint32_t)(x) & 0xFFu)
#define SAI_TCR2_BCD (1u << 24)
#define SAI_TCR2_BCP (1u << 25)
#define SAI_TCR2_MSEL(x) (((uint32_t)(x) & 0x3u) << 26)
#define SAI_TCR3_TCE(x)  (((uint32_t)(x) & 0xFu) << 16)
#define SAI_TCR4_FSD (1u << 0)
#define SAI_TCR4_FSP (1u << 1)
#define SAI_TCR4_FSE (1u << 3)
#define SAI_TCR4_MF  (1u << 4)
#define SAI_TCR4_SYWD(x) (((uint32_t)(x) & 0x1Fu) << 8)
#define SAI_TCR4_FRSZ(x) (((uint32_t)(x) & 0x1Fu) << 16)
#define SAI_TCR5_FBT(x)  (((uint32_t)(x) & 0x1Fu) << 8)
#define SAI_TCR5_W0W(x)  (((uint32_t)(x) & 0x1Fu) << 16)
#define SAI_TCR5_WNW(x)  (((uint32_t)(x) & 0x1Fu) << 24)
/* SAI1 clock: root 64 (mux 4 = Audio PLL, div 16), LPCG 123 */
#define CCM_CLOCK_ROOT64_CONTROL (*(volatile uint32_t *)0x40CC2000u)
#define CCM_LPCG123_DIRECT       (*(volatile uint32_t *)0x40CC6F60u)
/* SAI1 pins (ALT0, pad 0x02); TX are outputs, no select_input */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_17 (*(volatile uint32_t *)0x400E8150u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_21 (*(volatile uint32_t *)0x400E8160u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_22 (*(volatile uint32_t *)0x400E8164u)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_23 (*(volatile uint32_t *)0x400E8168u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_17 (*(volatile uint32_t *)0x400E8394u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_21 (*(volatile uint32_t *)0x400E83A4u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_22 (*(volatile uint32_t *)0x400E83A8u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_23 (*(volatile uint32_t *)0x400E83ACu)
#define IOMUXC_GPR_GPR0 (*(volatile uint32_t *)0x400E4000u)
#define IOMUXC_GPR_GPR0_SAI1_MCLK_DIR (1u << 8)
/* --- SAI1 RX (receive): registers mirror TX; bit positions identical --- */
#define SAI1_RCSR (*(volatile uint32_t *)0x40404088u)
#define SAI1_RCR1 (*(volatile uint32_t *)0x4040408Cu)
#define SAI1_RCR2 (*(volatile uint32_t *)0x40404090u)
#define SAI1_RCR3 (*(volatile uint32_t *)0x40404094u)
#define SAI1_RCR4 (*(volatile uint32_t *)0x40404098u)
#define SAI1_RCR5 (*(volatile uint32_t *)0x4040409Cu)
#define SAI1_RDR0 (*(volatile uint32_t *)0x404040A0u)
#define SAI1_RFR0 (*(volatile uint32_t *)0x404040C0u)
#define SAI1_RMR  (*(volatile uint32_t *)0x404040E0u)
#define SAI_RCSR_RE   (1u << 31)
#define SAI_RCSR_BCE  (1u << 28)
#define SAI_RCSR_FR   (1u << 25)
#define SAI_RCSR_SR   (1u << 24)
#define SAI_RCSR_FEF  (1u << 18)
#define SAI_RCSR_FWF  (1u << 17)
#define SAI_RCSR_FRF  (1u << 16)
#define SAI_RCSR_FRDE (1u << 0)
#define SAI_RCR2_SYNC(x) (((uint32_t)(x) & 0x3u) << 30)
#define SAI_RCR3_RCE(x)  (((uint32_t)(x) & 0xFu) << 16)
#define SAI_RCR4_FSP (1u << 1)
#define SAI_RCR4_FSE (1u << 3)
#define SAI_RCR4_MF  (1u << 4)
#define SAI_RCR4_SYWD(x) (((uint32_t)(x) & 0x1Fu) << 8)
#define SAI_RCR4_FRSZ(x) (((uint32_t)(x) & 0x1Fu) << 16)
#define SAI_RCR5_FBT(x)  (((uint32_t)(x) & 0x1Fu) << 8)
#define SAI_RCR5_W0W(x)  (((uint32_t)(x) & 0x1Fu) << 16)
#define SAI_RCR5_WNW(x)  (((uint32_t)(x) & 0x1Fu) << 24)
/* SAI1_RXD[0] = GPIO_AD_20 (ALT0); RX data is an input -> needs select_input */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_20 (*(volatile uint32_t *)0x400E815Cu)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_20 (*(volatile uint32_t *)0x400E83A0u)
#define IOMUXC_SAI1_RX_DATA0_SELECT_INPUT (*(volatile uint32_t *)0x400E8674u)

/* --- Audio PLL via ANATOP AI indirect interface (base 0x40C84000) --- */
#define ANADIG_PLL_AUDIO_CTRL (*(volatile uint32_t *)0x40C84300u)
#define ANADIG_PLL_AUDIO_CTRL_ENABLE_CLK (1u << 13)
#define ANADIG_PLL_AUDIO_CTRL_GATE       (1u << 14)
#define ANADIG_PLL_AUDIO_CTRL_STABLE     (1u << 29)
#define ANADIG_MISC_AI_CTRL_AUDIO  (*(volatile uint32_t *)0x40C84880u)
#define ANADIG_MISC_AI_WDATA_AUDIO (*(volatile uint32_t *)0x40C84890u)
#define ANADIG_MISC_AI_RDATA_AUDIO (*(volatile uint32_t *)0x40C848A0u)
#define ANADIG_AI_TOGGLE      (1u << 8)
#define ANADIG_AI_TOGGLE_DONE (1u << 9)
#define ANADIG_AI_RWB         (1u << 16)
#define ANADIG_AI_ADDR_MASK   (0xFFu)

/* --- VIDEO PLL via ANATOP AI indirect interface + PLL LDO (display clock) ---
 * The VIDEO PLL's real config (POWER_UP / BYPASS / HOLD_RING_OFF / DIV_SELECT /
 * NUM / DENOM) is NOT in the bus-visible PLL_VIDEO_CTRL (0x40C84350, in the
 * display register block below) -- it lives in AI-space CTRL0/CTRL2/CTRL3
 * reached only through this indirect interface, exactly like the AUDIO PLL
 * above.  fsl_clock.c CLOCK_InitVideoPll() + fsl_anatop_ai.c (kAI_Itf_Video).
 * The toggle/done/rwb/addr bits are shared with the audio interface. */
#define ANADIG_MISC_AI_CTRL_VIDEO  (*(volatile uint32_t *)0x40C848B0u)
#define ANADIG_MISC_AI_WDATA_VIDEO (*(volatile uint32_t *)0x40C848C0u)
#define ANADIG_MISC_AI_RDATA_VIDEO (*(volatile uint32_t *)0x40C848D0u)
/* LDO AI channel (VDDSOC): PMU_StaticEnablePllLdo drives the PHY_LDO through it;
 * the commit is a toggle of PMU_LDO_PLL[16], not a MISC AI_CTRL bit. */
#define ANADIG_MISC_AI_CTRL_LDO    (*(volatile uint32_t *)0x40C84820u)
#define ANADIG_MISC_AI_WDATA_LDO   (*(volatile uint32_t *)0x40C84830u)
#define ANADIG_MISC_AI_RDATA_LDO   (*(volatile uint32_t *)0x40C84840u)
#define ANADIG_PMU_LDO_PLL         (*(volatile uint32_t *)0x40C84500u)
#define ANADIG_PMU_LDO_PLL_AI_TOGGLE (1u << 16)
#define ANADIG_PMU_REF_CTRL        (*(volatile uint32_t *)0x40C84570u)
#define ANADIG_PMU_REF_CTRL_EN_PLL_VOL_REF_BUFFER (1u << 4)

/* --- LPI2C5 (codec bus, LPSR domain) base 0x40C34000, IRQ 36 --- */
#define LPI2C5_MCR    (*(volatile uint32_t *)0x40C34010u)
#define LPI2C5_MSR    (*(volatile uint32_t *)0x40C34014u)
#define LPI2C5_MIER   (*(volatile uint32_t *)0x40C34018u)
#define LPI2C5_MCFGR1 (*(volatile uint32_t *)0x40C34024u)
#define LPI2C5_MCCR0  (*(volatile uint32_t *)0x40C34048u)
#define LPI2C5_MFCR   (*(volatile uint32_t *)0x40C34058u)
#define LPI2C5_MTDR   (*(volatile uint32_t *)0x40C34060u)
#define LPI2C5_MRDR   (*(volatile uint32_t *)0x40C34070u)
#define LPI2C5_SCR    (*(volatile uint32_t *)0x40C34110u)
#define LPI2C5_SSR    (*(volatile uint32_t *)0x40C34114u)
#define LPI2C5_SIER   (*(volatile uint32_t *)0x40C34118u)
#define LPI2C5_SCFGR1 (*(volatile uint32_t *)0x40C34124u)
#define LPI2C5_SCFGR2 (*(volatile uint32_t *)0x40C34128u)
#define LPI2C5_SAMR   (*(volatile uint32_t *)0x40C34140u)
#define LPI2C5_SASR   (*(volatile uint32_t *)0x40C34150u)
#define LPI2C5_STAR   (*(volatile uint32_t *)0x40C34154u)
#define LPI2C5_STDR   (*(volatile uint32_t *)0x40C34160u)
#define LPI2C5_SRDR   (*(volatile uint32_t *)0x40C34170u)
/* LPI2C5 clock: root 41 (mux 1), LPCG 102 */
#define CCM_CLOCK_ROOT41_CONTROL (*(volatile uint32_t *)0x40CC1480u)
#define CCM_LPCG102_DIRECT       (*(volatile uint32_t *)0x40CC6CC0u)
/* LPI2C5 pins (LPSR IOMUXC 0x40C08000): SCL=LPSR_05, SDA=LPSR_04.
 * The LPSR_04/05 mux+pad macros ALREADY exist in the digital-pin block,
 * so only the two SELECT_INPUT (daisy) registers are added here. */
#define IOMUXC_LPI2C5_SCL_SELECT_INPUT (*(volatile uint32_t *)0x40C08084u)
#define IOMUXC_LPI2C5_SDA_SELECT_INPUT (*(volatile uint32_t *)0x40C08088u)

/* ---- eDMA + DMAMUX (classic Freescale eDMA: 32 channels, 16 IRQs) ----------
 * RT1176 uses the SAME IP as RT1062 (Teensy 4), relocated: eDMA 0x40070000,
 * DMAMUX 0x40074000.  cores/teensy4/DMAChannel.{h,cpp} ports with only base
 * retargets.  DMAMUX source numbers are RT1176-specific (NOT the RT1062 values;
 * verified against NXP PERI_DMAMUX.h and QEMU).  TCD0 at 0x40071000, stride 0x20.
 * Channel completion IRQs are NVIC 0..15 (channel n and n+16 share vector n);
 * DMA error is NVIC 16. */
/* eDMA control/status (base 0x40070000) */
#define DMA_CR       (*(volatile uint32_t *)0x40070000u)
#define DMA_ES       (*(volatile uint32_t *)0x40070004u)
#define DMA_ERQ      (*(volatile uint32_t *)0x4007000Cu)
#define DMA_EEI      (*(volatile uint32_t *)0x40070014u)
#define DMA_CEEI     (*(volatile uint8_t  *)0x40070018u)
#define DMA_SEEI     (*(volatile uint8_t  *)0x40070019u)
#define DMA_CERQ     (*(volatile uint8_t  *)0x4007001Au)
#define DMA_SERQ     (*(volatile uint8_t  *)0x4007001Bu)
#define DMA_CDNE     (*(volatile uint8_t  *)0x4007001Cu)
#define DMA_SSRT     (*(volatile uint8_t  *)0x4007001Du)
#define DMA_CERR     (*(volatile uint8_t  *)0x4007001Eu)
#define DMA_CINT     (*(volatile uint8_t  *)0x4007001Fu)
#define DMA_INT      (*(volatile uint32_t *)0x40070024u)
#define DMA_ERR      (*(volatile uint32_t *)0x4007002Cu)
#define DMA_HRS      (*(volatile uint32_t *)0x40070034u)
#define DMA_DCHPRI3  (*(volatile uint8_t  *)0x40070100u)   /* byte-swizzled priority array base */
/* DMA_CR bits */
#define DMA_CR_GRP1PRI ((uint32_t)(1<<10))
#define DMA_CR_EMLM    ((uint32_t)(1<<7))
#define DMA_CR_EDBG    ((uint32_t)(1<<1))
/* DMA_DCHPRIn bits */
#define DMA_DCHPRI_ECP ((uint8_t)(1<<7))
#define DMA_DCHPRI_DPA ((uint8_t)(1<<6))
/* TCD CSR bits */
#define DMA_TCD_CSR_DONE                 0x0080
#define DMA_TCD_CSR_MAJORELINK           0x0020
#define DMA_TCD_CSR_ESG                  0x0010
#define DMA_TCD_CSR_DREQ                 0x0008
#define DMA_TCD_CSR_INTHALF              0x0004
#define DMA_TCD_CSR_INTMAJOR             0x0002
#define DMA_TCD_CSR_START                0x0001
#define DMA_TCD_CSR_MAJORLINKCH(n)       (((n) & 0x1F) << 8)
#define DMA_TCD_CSR_MAJORLINKCH_MASK     0x1F00
/* TCD BITER linking bits */
#define DMA_TCD_BITER_ELINK                 ((uint16_t)1<<15)
#define DMA_TCD_BITER_ELINKYES_ELINK        0x8000
#define DMA_TCD_BITER_ELINKYES_LINKCH(n)    (((n) & 0x1F) << 9)
#define DMA_TCD_BITER_ELINKYES_LINKCH_MASK  0x3E00
/* DMAMUX (base 0x40074000): CHCFG0..31, 4 bytes apart */
#define DMAMUX_CHCFG0      (*(volatile uint32_t *)0x40074000u)
#define DMAMUX_CHCFG_ENBL  ((uint32_t)(1<<31))
#define DMAMUX_CHCFG_TRIG  ((uint32_t)(1<<30))
#define DMAMUX_CHCFG_A_ON  ((uint32_t)(1<<29))
/* eDMA clock gate: CCM LPCG22 (kCLOCK_Edma=22 -> 0x40CC6000 + 22*0x20). */
#define CCM_LPCG22_DIRECT  (*(volatile uint32_t *)0x40CC62C0u)
/* DMAMUX request sources (RT1176 silicon; NXP PERI_DMAMUX.h; matches QEMU wiring) */
#define DMAMUX_SOURCE_LPUART1_TX 8
#define DMAMUX_SOURCE_LPUART1_RX 9
#define DMAMUX_SOURCE_LPSPI1_RX  36
#define DMAMUX_SOURCE_LPSPI1_TX  37
#define DMAMUX_SOURCE_LPI2C1     48
#define DMAMUX_SOURCE_SAI1_RX    54
#define DMAMUX_SOURCE_SAI1_TX    55

/* FlexSPI1 IP-command registers (for the flash-emulated EEPROM program/erase
 * primitives in cores/imxrt1176/eeprom.c).  Same IP + offsets as the RT1062
 * FlexSPI in cores/teensy4/imxrt.h, retargeted to FlexSPI1 base 0x400CC000. */
#define FLEXSPI_MCR0     (*(volatile uint32_t *)(FLEXSPI_BASE + 0x000))
#define FLEXSPI_MCR0_SWRESET     ((uint32_t)(1<<0))
#define FLEXSPI_INTR     (*(volatile uint32_t *)(FLEXSPI_BASE + 0x014))
#define FLEXSPI_INTR_IPCMDDONE   ((uint32_t)(1<<0))
#define FLEXSPI_INTR_IPTXWE      ((uint32_t)(1<<6))
#define FLEXSPI_LUTKEY   (*(volatile uint32_t *)(FLEXSPI_BASE + 0x018))
#define FLEXSPI_LUTKEY_VALUE     ((uint32_t)0x5AF05AF0)
#define FLEXSPI_LUTCR    (*(volatile uint32_t *)(FLEXSPI_BASE + 0x01C))
#define FLEXSPI_LUTCR_UNLOCK     ((uint32_t)(1<<1))
#define FLEXSPI_IPCR0    (*(volatile uint32_t *)(FLEXSPI_BASE + 0x0A0))
#define FLEXSPI_IPCR1    (*(volatile uint32_t *)(FLEXSPI_BASE + 0x0A4))
#define FLEXSPI_IPCR1_ISEQID(n)  ((uint32_t)(((n) & 0x0F) << 16))
#define FLEXSPI_IPCR1_IDATSZ(n)  ((uint32_t)(((n) & 0xFFFF) << 0))
#define FLEXSPI_IPCMD    (*(volatile uint32_t *)(FLEXSPI_BASE + 0x0B0))
#define FLEXSPI_IPCMD_TRG        ((uint32_t)(1<<0))
#define FLEXSPI_IPRXFCR  (*(volatile uint32_t *)(FLEXSPI_BASE + 0x0B8))
#define FLEXSPI_IPRXFCR_CLRIPRXF ((uint32_t)(1<<0))
#define FLEXSPI_IPTXFCR  (*(volatile uint32_t *)(FLEXSPI_BASE + 0x0BC))
#define FLEXSPI_IPTXFCR_CLRIPTXF ((uint32_t)(1<<0))
#define FLEXSPI_RFDR0    (*(volatile uint32_t *)(FLEXSPI_BASE + 0x100))
#define FLEXSPI_TFDR0    (*(volatile uint32_t *)(FLEXSPI_BASE + 0x180))
#define FLEXSPI_LUT60    (*(volatile uint32_t *)(FLEXSPI_BASE + 0x2F0))
#define FLEXSPI_LUT61    (*(volatile uint32_t *)(FLEXSPI_BASE + 0x2F4))
#define FLEXSPI_LUT62    (*(volatile uint32_t *)(FLEXSPI_BASE + 0x2F8))
#define FLEXSPI_LUT63    (*(volatile uint32_t *)(FLEXSPI_BASE + 0x2FC))
#define FLEXSPI_LUT_INSTRUCTION(opcode, pads, operand) ((uint32_t)(\
	(((opcode) & 0x3F) << 10) | (((pads) & 0x03) << 8) | ((operand) & 0xFF)))
#define FLEXSPI_LUT_OPCODE_CMD_SDR   0x01
#define FLEXSPI_LUT_OPCODE_RADDR_SDR 0x02
#define FLEXSPI_LUT_OPCODE_WRITE_SDR 0x08
#define FLEXSPI_LUT_OPCODE_READ_SDR  0x09
#define FLEXSPI_LUT_NUM_PADS_1       0x00
#define FLEXSPI_LUT_NUM_PADS_4       0x02

/* ==== SEMC (external SDRAM controller) @ CS0 0x80000000 ==== */
#define SEMC_BASE 0x400D4000u
#define SEMC_MCR (*(volatile uint32_t *)(SEMC_BASE + 0x00u))
#define SEMC_IOCR (*(volatile uint32_t *)(SEMC_BASE + 0x04u))
#define SEMC_BMCR0 (*(volatile uint32_t *)(SEMC_BASE + 0x08u))
#define SEMC_BMCR1 (*(volatile uint32_t *)(SEMC_BASE + 0x0Cu))
#define SEMC_BR0 (*(volatile uint32_t *)(SEMC_BASE + 0x10u))
#define SEMC_BR1 (*(volatile uint32_t *)(SEMC_BASE + 0x14u))
#define SEMC_BR2 (*(volatile uint32_t *)(SEMC_BASE + 0x18u))
#define SEMC_BR3 (*(volatile uint32_t *)(SEMC_BASE + 0x1Cu))
#define SEMC_BR4 (*(volatile uint32_t *)(SEMC_BASE + 0x20u))
#define SEMC_BR5 (*(volatile uint32_t *)(SEMC_BASE + 0x24u))
#define SEMC_BR6 (*(volatile uint32_t *)(SEMC_BASE + 0x28u))
#define SEMC_BR7 (*(volatile uint32_t *)(SEMC_BASE + 0x2Cu))
#define SEMC_BR8 (*(volatile uint32_t *)(SEMC_BASE + 0x30u))
#define SEMC_INTEN (*(volatile uint32_t *)(SEMC_BASE + 0x38u))
#define SEMC_INTR (*(volatile uint32_t *)(SEMC_BASE + 0x3Cu))
#define SEMC_SDRAMCR0 (*(volatile uint32_t *)(SEMC_BASE + 0x40u))
#define SEMC_SDRAMCR1 (*(volatile uint32_t *)(SEMC_BASE + 0x44u))
#define SEMC_SDRAMCR2 (*(volatile uint32_t *)(SEMC_BASE + 0x48u))
#define SEMC_SDRAMCR3 (*(volatile uint32_t *)(SEMC_BASE + 0x4Cu))
#define SEMC_DBICR0 (*(volatile uint32_t *)(SEMC_BASE + 0x80u))
#define SEMC_DBICR1 (*(volatile uint32_t *)(SEMC_BASE + 0x84u))
#define SEMC_IPCR0 (*(volatile uint32_t *)(SEMC_BASE + 0x90u))
#define SEMC_IPCR1 (*(volatile uint32_t *)(SEMC_BASE + 0x94u))
#define SEMC_IPCR2 (*(volatile uint32_t *)(SEMC_BASE + 0x98u))
#define SEMC_IPCMD (*(volatile uint32_t *)(SEMC_BASE + 0x9Cu))
#define SEMC_IPTXDAT (*(volatile uint32_t *)(SEMC_BASE + 0xA0u))
#define SEMC_IPRXDAT (*(volatile uint32_t *)(SEMC_BASE + 0xB0u))
#define SEMC_STS0 (*(volatile uint32_t *)(SEMC_BASE + 0xC0u))
#define SEMC_STS1 (*(volatile uint32_t *)(SEMC_BASE + 0xC4u))
#define SEMC_STS2 (*(volatile uint32_t *)(SEMC_BASE + 0xC8u))
#define SEMC_STS3 (*(volatile uint32_t *)(SEMC_BASE + 0xCCu))
#define SEMC_STS4 (*(volatile uint32_t *)(SEMC_BASE + 0xD0u))
#define SEMC_STS5 (*(volatile uint32_t *)(SEMC_BASE + 0xD4u))
#define SEMC_STS6 (*(volatile uint32_t *)(SEMC_BASE + 0xD8u))
#define SEMC_STS7 (*(volatile uint32_t *)(SEMC_BASE + 0xDCu))
#define SEMC_STS8 (*(volatile uint32_t *)(SEMC_BASE + 0xE0u))
#define SEMC_STS9 (*(volatile uint32_t *)(SEMC_BASE + 0xE4u))
#define SEMC_STS10 (*(volatile uint32_t *)(SEMC_BASE + 0xE8u))
#define SEMC_STS11 (*(volatile uint32_t *)(SEMC_BASE + 0xECu))
#define SEMC_STS12 (*(volatile uint32_t *)(SEMC_BASE + 0xF0u))
#define SEMC_DCCR (*(volatile uint32_t *)(SEMC_BASE + 0x150u))
#define SEMC_BR(n) (*(volatile uint32_t *)(SEMC_BASE + 0x10u + ((n) << 2)))  /* BR[0..8] */

/* SEMC register bitfields (verbatim from NXP PERI_SEMC.h) */
#define SEMC_MCR_SWRST_MASK (0x1U)
#define SEMC_MCR_SWRST_SHIFT (0U)
#define SEMC_MCR_SWRST(x) (((uint32_t)(((uint32_t)(x)) << SEMC_MCR_SWRST_SHIFT)) & SEMC_MCR_SWRST_MASK)
#define SEMC_MCR_MDIS_MASK (0x2U)
#define SEMC_MCR_MDIS_SHIFT (1U)
#define SEMC_MCR_MDIS(x) (((uint32_t)(((uint32_t)(x)) << SEMC_MCR_MDIS_SHIFT)) & SEMC_MCR_MDIS_MASK)
#define SEMC_MCR_DQSMD_MASK (0x4U)
#define SEMC_MCR_DQSMD_SHIFT (2U)
#define SEMC_MCR_DQSMD(x) (((uint32_t)(((uint32_t)(x)) << SEMC_MCR_DQSMD_SHIFT)) & SEMC_MCR_DQSMD_MASK)
#define SEMC_MCR_WPOL0_MASK (0x40U)
#define SEMC_MCR_WPOL0_SHIFT (6U)
#define SEMC_MCR_WPOL0(x) (((uint32_t)(((uint32_t)(x)) << SEMC_MCR_WPOL0_SHIFT)) & SEMC_MCR_WPOL0_MASK)
#define SEMC_MCR_WPOL1_MASK (0x80U)
#define SEMC_MCR_WPOL1_SHIFT (7U)
#define SEMC_MCR_WPOL1(x) (((uint32_t)(((uint32_t)(x)) << SEMC_MCR_WPOL1_SHIFT)) & SEMC_MCR_WPOL1_MASK)
#define SEMC_MCR_CTO_MASK (0xFF0000U)
#define SEMC_MCR_CTO_SHIFT (16U)
#define SEMC_MCR_CTO(x) (((uint32_t)(((uint32_t)(x)) << SEMC_MCR_CTO_SHIFT)) & SEMC_MCR_CTO_MASK)
#define SEMC_MCR_BTO_MASK (0x1F000000U)
#define SEMC_MCR_BTO_SHIFT (24U)
#define SEMC_MCR_BTO(x) (((uint32_t)(((uint32_t)(x)) << SEMC_MCR_BTO_SHIFT)) & SEMC_MCR_BTO_MASK)
#define SEMC_IOCR_MUX_A8_MASK (0xFU)
#define SEMC_IOCR_MUX_A8_SHIFT (0U)
#define SEMC_IOCR_MUX_A8(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IOCR_MUX_A8_SHIFT)) & SEMC_IOCR_MUX_A8_MASK)
#define SEMC_IOCR_MUX_CSX0_MASK (0xF0U)
#define SEMC_IOCR_MUX_CSX0_SHIFT (4U)
#define SEMC_IOCR_MUX_CSX0(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IOCR_MUX_CSX0_SHIFT)) & SEMC_IOCR_MUX_CSX0_MASK)
#define SEMC_IOCR_MUX_CSX1_MASK (0xF00U)
#define SEMC_IOCR_MUX_CSX1_SHIFT (8U)
#define SEMC_IOCR_MUX_CSX1(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IOCR_MUX_CSX1_SHIFT)) & SEMC_IOCR_MUX_CSX1_MASK)
#define SEMC_IOCR_MUX_CSX2_MASK (0xF000U)
#define SEMC_IOCR_MUX_CSX2_SHIFT (12U)
#define SEMC_IOCR_MUX_CSX2(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IOCR_MUX_CSX2_SHIFT)) & SEMC_IOCR_MUX_CSX2_MASK)
#define SEMC_IOCR_MUX_CSX3_MASK (0xF0000U)
#define SEMC_IOCR_MUX_CSX3_SHIFT (16U)
#define SEMC_IOCR_MUX_CSX3(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IOCR_MUX_CSX3_SHIFT)) & SEMC_IOCR_MUX_CSX3_MASK)
#define SEMC_IOCR_MUX_RDY_MASK (0xF00000U)
#define SEMC_IOCR_MUX_RDY_SHIFT (20U)
#define SEMC_IOCR_MUX_RDY(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IOCR_MUX_RDY_SHIFT)) & SEMC_IOCR_MUX_RDY_MASK)
#define SEMC_IOCR_MUX_CLKX0_MASK (0x3000000U)
#define SEMC_IOCR_MUX_CLKX0_SHIFT (24U)
#define SEMC_IOCR_MUX_CLKX0(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IOCR_MUX_CLKX0_SHIFT)) & SEMC_IOCR_MUX_CLKX0_MASK)
#define SEMC_IOCR_MUX_CLKX1_MASK (0xC000000U)
#define SEMC_IOCR_MUX_CLKX1_SHIFT (26U)
#define SEMC_IOCR_MUX_CLKX1(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IOCR_MUX_CLKX1_SHIFT)) & SEMC_IOCR_MUX_CLKX1_MASK)
#define SEMC_IOCR_CLKX0_AO_MASK (0x10000000U)
#define SEMC_IOCR_CLKX0_AO_SHIFT (28U)
#define SEMC_IOCR_CLKX0_AO(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IOCR_CLKX0_AO_SHIFT)) & SEMC_IOCR_CLKX0_AO_MASK)
#define SEMC_IOCR_CLKX1_AO_MASK (0x20000000U)
#define SEMC_IOCR_CLKX1_AO_SHIFT (29U)
#define SEMC_IOCR_CLKX1_AO(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IOCR_CLKX1_AO_SHIFT)) & SEMC_IOCR_CLKX1_AO_MASK)
#define SEMC_BMCR0_WQOS_MASK (0xFU)
#define SEMC_BMCR0_WQOS_SHIFT (0U)
#define SEMC_BMCR0_WQOS(x) (((uint32_t)(((uint32_t)(x)) << SEMC_BMCR0_WQOS_SHIFT)) & SEMC_BMCR0_WQOS_MASK)
#define SEMC_BMCR0_WAGE_MASK (0xF0U)
#define SEMC_BMCR0_WAGE_SHIFT (4U)
#define SEMC_BMCR0_WAGE(x) (((uint32_t)(((uint32_t)(x)) << SEMC_BMCR0_WAGE_SHIFT)) & SEMC_BMCR0_WAGE_MASK)
#define SEMC_BMCR0_WSH_MASK (0xFF00U)
#define SEMC_BMCR0_WSH_SHIFT (8U)
#define SEMC_BMCR0_WSH(x) (((uint32_t)(((uint32_t)(x)) << SEMC_BMCR0_WSH_SHIFT)) & SEMC_BMCR0_WSH_MASK)
#define SEMC_BMCR0_WRWS_MASK (0xFF0000U)
#define SEMC_BMCR0_WRWS_SHIFT (16U)
#define SEMC_BMCR0_WRWS(x) (((uint32_t)(((uint32_t)(x)) << SEMC_BMCR0_WRWS_SHIFT)) & SEMC_BMCR0_WRWS_MASK)
#define SEMC_BMCR1_WQOS_MASK (0xFU)
#define SEMC_BMCR1_WQOS_SHIFT (0U)
#define SEMC_BMCR1_WQOS(x) (((uint32_t)(((uint32_t)(x)) << SEMC_BMCR1_WQOS_SHIFT)) & SEMC_BMCR1_WQOS_MASK)
#define SEMC_BMCR1_WAGE_MASK (0xF0U)
#define SEMC_BMCR1_WAGE_SHIFT (4U)
#define SEMC_BMCR1_WAGE(x) (((uint32_t)(((uint32_t)(x)) << SEMC_BMCR1_WAGE_SHIFT)) & SEMC_BMCR1_WAGE_MASK)
#define SEMC_BMCR1_WPH_MASK (0xFF00U)
#define SEMC_BMCR1_WPH_SHIFT (8U)
#define SEMC_BMCR1_WPH(x) (((uint32_t)(((uint32_t)(x)) << SEMC_BMCR1_WPH_SHIFT)) & SEMC_BMCR1_WPH_MASK)
#define SEMC_BMCR1_WRWS_MASK (0xFF0000U)
#define SEMC_BMCR1_WRWS_SHIFT (16U)
#define SEMC_BMCR1_WRWS(x) (((uint32_t)(((uint32_t)(x)) << SEMC_BMCR1_WRWS_SHIFT)) & SEMC_BMCR1_WRWS_MASK)
#define SEMC_BMCR1_WBR_MASK (0xFF000000U)
#define SEMC_BMCR1_WBR_SHIFT (24U)
#define SEMC_BMCR1_WBR(x) (((uint32_t)(((uint32_t)(x)) << SEMC_BMCR1_WBR_SHIFT)) & SEMC_BMCR1_WBR_MASK)
#define SEMC_BR_VLD_MASK (0x1U)
#define SEMC_BR_VLD_SHIFT (0U)
#define SEMC_BR_VLD(x) (((uint32_t)(((uint32_t)(x)) << SEMC_BR_VLD_SHIFT)) & SEMC_BR_VLD_MASK)
#define SEMC_BR_MS_MASK (0x3EU)
#define SEMC_BR_MS_SHIFT (1U)
#define SEMC_BR_MS(x) (((uint32_t)(((uint32_t)(x)) << SEMC_BR_MS_SHIFT)) & SEMC_BR_MS_MASK)
#define SEMC_BR_BA_MASK (0xFFFFF000U)
#define SEMC_BR_BA_SHIFT (12U)
#define SEMC_BR_BA(x) (((uint32_t)(((uint32_t)(x)) << SEMC_BR_BA_SHIFT)) & SEMC_BR_BA_MASK)
#define SEMC_INTEN_IPCMDDONEEN_MASK (0x1U)
#define SEMC_INTEN_IPCMDDONEEN_SHIFT (0U)
#define SEMC_INTEN_IPCMDDONEEN(x) (((uint32_t)(((uint32_t)(x)) << SEMC_INTEN_IPCMDDONEEN_SHIFT)) & SEMC_INTEN_IPCMDDONEEN_MASK)
#define SEMC_INTEN_IPCMDERREN_MASK (0x2U)
#define SEMC_INTEN_IPCMDERREN_SHIFT (1U)
#define SEMC_INTEN_IPCMDERREN(x) (((uint32_t)(((uint32_t)(x)) << SEMC_INTEN_IPCMDERREN_SHIFT)) & SEMC_INTEN_IPCMDERREN_MASK)
#define SEMC_INTEN_AXICMDERREN_MASK (0x4U)
#define SEMC_INTEN_AXICMDERREN_SHIFT (2U)
#define SEMC_INTEN_AXICMDERREN(x) (((uint32_t)(((uint32_t)(x)) << SEMC_INTEN_AXICMDERREN_SHIFT)) & SEMC_INTEN_AXICMDERREN_MASK)
#define SEMC_INTEN_AXIBUSERREN_MASK (0x8U)
#define SEMC_INTEN_AXIBUSERREN_SHIFT (3U)
#define SEMC_INTEN_AXIBUSERREN(x) (((uint32_t)(((uint32_t)(x)) << SEMC_INTEN_AXIBUSERREN_SHIFT)) & SEMC_INTEN_AXIBUSERREN_MASK)
#define SEMC_INTEN_NDPAGEENDEN_MASK (0x10U)
#define SEMC_INTEN_NDPAGEENDEN_SHIFT (4U)
#define SEMC_INTEN_NDPAGEENDEN(x) (((uint32_t)(((uint32_t)(x)) << SEMC_INTEN_NDPAGEENDEN_SHIFT)) & SEMC_INTEN_NDPAGEENDEN_MASK)
#define SEMC_INTEN_NDNOPENDEN_MASK (0x20U)
#define SEMC_INTEN_NDNOPENDEN_SHIFT (5U)
#define SEMC_INTEN_NDNOPENDEN(x) (((uint32_t)(((uint32_t)(x)) << SEMC_INTEN_NDNOPENDEN_SHIFT)) & SEMC_INTEN_NDNOPENDEN_MASK)
#define SEMC_INTR_IPCMDDONE_MASK (0x1U)
#define SEMC_INTR_IPCMDDONE_SHIFT (0U)
#define SEMC_INTR_IPCMDDONE(x) (((uint32_t)(((uint32_t)(x)) << SEMC_INTR_IPCMDDONE_SHIFT)) & SEMC_INTR_IPCMDDONE_MASK)
#define SEMC_INTR_IPCMDERR_MASK (0x2U)
#define SEMC_INTR_IPCMDERR_SHIFT (1U)
#define SEMC_INTR_IPCMDERR(x) (((uint32_t)(((uint32_t)(x)) << SEMC_INTR_IPCMDERR_SHIFT)) & SEMC_INTR_IPCMDERR_MASK)
#define SEMC_INTR_AXICMDERR_MASK (0x4U)
#define SEMC_INTR_AXICMDERR_SHIFT (2U)
#define SEMC_INTR_AXICMDERR(x) (((uint32_t)(((uint32_t)(x)) << SEMC_INTR_AXICMDERR_SHIFT)) & SEMC_INTR_AXICMDERR_MASK)
#define SEMC_INTR_AXIBUSERR_MASK (0x8U)
#define SEMC_INTR_AXIBUSERR_SHIFT (3U)
#define SEMC_INTR_AXIBUSERR(x) (((uint32_t)(((uint32_t)(x)) << SEMC_INTR_AXIBUSERR_SHIFT)) & SEMC_INTR_AXIBUSERR_MASK)
#define SEMC_INTR_NDPAGEEND_MASK (0x10U)
#define SEMC_INTR_NDPAGEEND_SHIFT (4U)
#define SEMC_INTR_NDPAGEEND(x) (((uint32_t)(((uint32_t)(x)) << SEMC_INTR_NDPAGEEND_SHIFT)) & SEMC_INTR_NDPAGEEND_MASK)
#define SEMC_INTR_NDNOPEND_MASK (0x20U)
#define SEMC_INTR_NDNOPEND_SHIFT (5U)
#define SEMC_INTR_NDNOPEND(x) (((uint32_t)(((uint32_t)(x)) << SEMC_INTR_NDNOPEND_SHIFT)) & SEMC_INTR_NDNOPEND_MASK)
#define SEMC_SDRAMCR0_PS_MASK (0x3U)
#define SEMC_SDRAMCR0_PS_SHIFT (0U)
#define SEMC_SDRAMCR0_PS(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR0_PS_SHIFT)) & SEMC_SDRAMCR0_PS_MASK)
#define SEMC_SDRAMCR0_BL_MASK (0x70U)
#define SEMC_SDRAMCR0_BL_SHIFT (4U)
#define SEMC_SDRAMCR0_BL(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR0_BL_SHIFT)) & SEMC_SDRAMCR0_BL_MASK)
#define SEMC_SDRAMCR0_COL8_MASK (0x80U)
#define SEMC_SDRAMCR0_COL8_SHIFT (7U)
#define SEMC_SDRAMCR0_COL8(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR0_COL8_SHIFT)) & SEMC_SDRAMCR0_COL8_MASK)
#define SEMC_SDRAMCR0_COL_MASK (0x300U)
#define SEMC_SDRAMCR0_COL_SHIFT (8U)
#define SEMC_SDRAMCR0_COL(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR0_COL_SHIFT)) & SEMC_SDRAMCR0_COL_MASK)
#define SEMC_SDRAMCR0_CL_MASK (0xC00U)
#define SEMC_SDRAMCR0_CL_SHIFT (10U)
#define SEMC_SDRAMCR0_CL(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR0_CL_SHIFT)) & SEMC_SDRAMCR0_CL_MASK)
#define SEMC_SDRAMCR0_BANK2_MASK (0x4000U)
#define SEMC_SDRAMCR0_BANK2_SHIFT (14U)
#define SEMC_SDRAMCR0_BANK2(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR0_BANK2_SHIFT)) & SEMC_SDRAMCR0_BANK2_MASK)
#define SEMC_SDRAMCR1_PRE2ACT_MASK (0xFU)
#define SEMC_SDRAMCR1_PRE2ACT_SHIFT (0U)
#define SEMC_SDRAMCR1_PRE2ACT(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR1_PRE2ACT_SHIFT)) & SEMC_SDRAMCR1_PRE2ACT_MASK)
#define SEMC_SDRAMCR1_ACT2RW_MASK (0xF0U)
#define SEMC_SDRAMCR1_ACT2RW_SHIFT (4U)
#define SEMC_SDRAMCR1_ACT2RW(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR1_ACT2RW_SHIFT)) & SEMC_SDRAMCR1_ACT2RW_MASK)
#define SEMC_SDRAMCR1_RFRC_MASK (0x1F00U)
#define SEMC_SDRAMCR1_RFRC_SHIFT (8U)
#define SEMC_SDRAMCR1_RFRC(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR1_RFRC_SHIFT)) & SEMC_SDRAMCR1_RFRC_MASK)
#define SEMC_SDRAMCR1_WRC_MASK (0xE000U)
#define SEMC_SDRAMCR1_WRC_SHIFT (13U)
#define SEMC_SDRAMCR1_WRC(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR1_WRC_SHIFT)) & SEMC_SDRAMCR1_WRC_MASK)
#define SEMC_SDRAMCR1_CKEOFF_MASK (0xF0000U)
#define SEMC_SDRAMCR1_CKEOFF_SHIFT (16U)
#define SEMC_SDRAMCR1_CKEOFF(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR1_CKEOFF_SHIFT)) & SEMC_SDRAMCR1_CKEOFF_MASK)
#define SEMC_SDRAMCR1_ACT2PRE_MASK (0xF00000U)
#define SEMC_SDRAMCR1_ACT2PRE_SHIFT (20U)
#define SEMC_SDRAMCR1_ACT2PRE(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR1_ACT2PRE_SHIFT)) & SEMC_SDRAMCR1_ACT2PRE_MASK)
#define SEMC_SDRAMCR2_SRRC_MASK (0xFFU)
#define SEMC_SDRAMCR2_SRRC_SHIFT (0U)
#define SEMC_SDRAMCR2_SRRC(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR2_SRRC_SHIFT)) & SEMC_SDRAMCR2_SRRC_MASK)
#define SEMC_SDRAMCR2_REF2REF_MASK (0xFF00U)
#define SEMC_SDRAMCR2_REF2REF_SHIFT (8U)
#define SEMC_SDRAMCR2_REF2REF(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR2_REF2REF_SHIFT)) & SEMC_SDRAMCR2_REF2REF_MASK)
#define SEMC_SDRAMCR2_ACT2ACT_MASK (0xFF0000U)
#define SEMC_SDRAMCR2_ACT2ACT_SHIFT (16U)
#define SEMC_SDRAMCR2_ACT2ACT(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR2_ACT2ACT_SHIFT)) & SEMC_SDRAMCR2_ACT2ACT_MASK)
#define SEMC_SDRAMCR2_ITO_MASK (0xFF000000U)
#define SEMC_SDRAMCR2_ITO_SHIFT (24U)
#define SEMC_SDRAMCR2_ITO(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR2_ITO_SHIFT)) & SEMC_SDRAMCR2_ITO_MASK)
#define SEMC_SDRAMCR3_REN_MASK (0x1U)
#define SEMC_SDRAMCR3_REN_SHIFT (0U)
#define SEMC_SDRAMCR3_REN(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR3_REN_SHIFT)) & SEMC_SDRAMCR3_REN_MASK)
#define SEMC_SDRAMCR3_REBL_MASK (0xEU)
#define SEMC_SDRAMCR3_REBL_SHIFT (1U)
#define SEMC_SDRAMCR3_REBL(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR3_REBL_SHIFT)) & SEMC_SDRAMCR3_REBL_MASK)
#define SEMC_SDRAMCR3_PRESCALE_MASK (0xFF00U)
#define SEMC_SDRAMCR3_PRESCALE_SHIFT (8U)
#define SEMC_SDRAMCR3_PRESCALE(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR3_PRESCALE_SHIFT)) & SEMC_SDRAMCR3_PRESCALE_MASK)
#define SEMC_SDRAMCR3_RT_MASK (0xFF0000U)
#define SEMC_SDRAMCR3_RT_SHIFT (16U)
#define SEMC_SDRAMCR3_RT(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR3_RT_SHIFT)) & SEMC_SDRAMCR3_RT_MASK)
#define SEMC_SDRAMCR3_UT_MASK (0xFF000000U)
#define SEMC_SDRAMCR3_UT_SHIFT (24U)
#define SEMC_SDRAMCR3_UT(x) (((uint32_t)(((uint32_t)(x)) << SEMC_SDRAMCR3_UT_SHIFT)) & SEMC_SDRAMCR3_UT_MASK)
#define SEMC_IPCR0_SA_MASK (0xFFFFFFFFU)
#define SEMC_IPCR0_SA_SHIFT (0U)
#define SEMC_IPCR0_SA(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IPCR0_SA_SHIFT)) & SEMC_IPCR0_SA_MASK)
#define SEMC_IPCR1_DATSZ_MASK (0x7U)
#define SEMC_IPCR1_DATSZ_SHIFT (0U)
#define SEMC_IPCR1_DATSZ(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IPCR1_DATSZ_SHIFT)) & SEMC_IPCR1_DATSZ_MASK)
#define SEMC_IPCR1_NAND_EXT_ADDR_MASK (0xFF00U)
#define SEMC_IPCR1_NAND_EXT_ADDR_SHIFT (8U)
#define SEMC_IPCR1_NAND_EXT_ADDR(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IPCR1_NAND_EXT_ADDR_SHIFT)) & SEMC_IPCR1_NAND_EXT_ADDR_MASK)
#define SEMC_IPCR2_BM0_MASK (0x1U)
#define SEMC_IPCR2_BM0_SHIFT (0U)
#define SEMC_IPCR2_BM0(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IPCR2_BM0_SHIFT)) & SEMC_IPCR2_BM0_MASK)
#define SEMC_IPCR2_BM1_MASK (0x2U)
#define SEMC_IPCR2_BM1_SHIFT (1U)
#define SEMC_IPCR2_BM1(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IPCR2_BM1_SHIFT)) & SEMC_IPCR2_BM1_MASK)
#define SEMC_IPCR2_BM2_MASK (0x4U)
#define SEMC_IPCR2_BM2_SHIFT (2U)
#define SEMC_IPCR2_BM2(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IPCR2_BM2_SHIFT)) & SEMC_IPCR2_BM2_MASK)
#define SEMC_IPCR2_BM3_MASK (0x8U)
#define SEMC_IPCR2_BM3_SHIFT (3U)
#define SEMC_IPCR2_BM3(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IPCR2_BM3_SHIFT)) & SEMC_IPCR2_BM3_MASK)
#define SEMC_IPCMD_CMD_MASK (0xFFFFU)
#define SEMC_IPCMD_CMD_SHIFT (0U)
#define SEMC_IPCMD_CMD(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IPCMD_CMD_SHIFT)) & SEMC_IPCMD_CMD_MASK)
#define SEMC_IPCMD_KEY_MASK (0xFFFF0000U)
#define SEMC_IPCMD_KEY_SHIFT (16U)
#define SEMC_IPCMD_KEY(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IPCMD_KEY_SHIFT)) & SEMC_IPCMD_KEY_MASK)
#define SEMC_IPTXDAT_DAT_MASK (0xFFFFFFFFU)
#define SEMC_IPTXDAT_DAT_SHIFT (0U)
#define SEMC_IPTXDAT_DAT(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IPTXDAT_DAT_SHIFT)) & SEMC_IPTXDAT_DAT_MASK)
#define SEMC_IPRXDAT_DAT_MASK (0xFFFFFFFFU)
#define SEMC_IPRXDAT_DAT_SHIFT (0U)
#define SEMC_IPRXDAT_DAT(x) (((uint32_t)(((uint32_t)(x)) << SEMC_IPRXDAT_DAT_SHIFT)) & SEMC_IPRXDAT_DAT_MASK)
#define SEMC_STS0_IDLE_MASK (0x1U)
#define SEMC_STS0_IDLE_SHIFT (0U)
#define SEMC_STS0_IDLE(x) (((uint32_t)(((uint32_t)(x)) << SEMC_STS0_IDLE_SHIFT)) & SEMC_STS0_IDLE_MASK)
#define SEMC_STS0_NARDY_MASK (0x2U)
#define SEMC_STS0_NARDY_SHIFT (1U)
#define SEMC_STS0_NARDY(x) (((uint32_t)(((uint32_t)(x)) << SEMC_STS0_NARDY_SHIFT)) & SEMC_STS0_NARDY_MASK)
#define SEMC_DCCR_SDRAMEN_MASK (0x1U)
#define SEMC_DCCR_SDRAMEN_SHIFT (0U)
#define SEMC_DCCR_SDRAMEN(x) (((uint32_t)(((uint32_t)(x)) << SEMC_DCCR_SDRAMEN_SHIFT)) & SEMC_DCCR_SDRAMEN_MASK)
#define SEMC_DCCR_SDRAMVAL_MASK (0x3EU)
#define SEMC_DCCR_SDRAMVAL_SHIFT (1U)
#define SEMC_DCCR_SDRAMVAL(x) (((uint32_t)(((uint32_t)(x)) << SEMC_DCCR_SDRAMVAL_SHIFT)) & SEMC_DCCR_SDRAMVAL_MASK)
#define SEMC_DCCR_NOREN_MASK (0x100U)
#define SEMC_DCCR_NOREN_SHIFT (8U)
#define SEMC_DCCR_NOREN(x) (((uint32_t)(((uint32_t)(x)) << SEMC_DCCR_NOREN_SHIFT)) & SEMC_DCCR_NOREN_MASK)
#define SEMC_DCCR_NORVAL_MASK (0x3E00U)
#define SEMC_DCCR_NORVAL_SHIFT (9U)
#define SEMC_DCCR_NORVAL(x) (((uint32_t)(((uint32_t)(x)) << SEMC_DCCR_NORVAL_SHIFT)) & SEMC_DCCR_NORVAL_MASK)
#define SEMC_DCCR_SRAM0EN_MASK (0x10000U)
#define SEMC_DCCR_SRAM0EN_SHIFT (16U)
#define SEMC_DCCR_SRAM0EN(x) (((uint32_t)(((uint32_t)(x)) << SEMC_DCCR_SRAM0EN_SHIFT)) & SEMC_DCCR_SRAM0EN_MASK)
#define SEMC_DCCR_SRAM0VAL_MASK (0x3E0000U)
#define SEMC_DCCR_SRAM0VAL_SHIFT (17U)
#define SEMC_DCCR_SRAM0VAL(x) (((uint32_t)(((uint32_t)(x)) << SEMC_DCCR_SRAM0VAL_SHIFT)) & SEMC_DCCR_SRAM0VAL_MASK)
#define SEMC_DCCR_SRAMXEN_MASK (0x1000000U)
#define SEMC_DCCR_SRAMXEN_SHIFT (24U)
#define SEMC_DCCR_SRAMXEN(x) (((uint32_t)(((uint32_t)(x)) << SEMC_DCCR_SRAMXEN_SHIFT)) & SEMC_DCCR_SRAMXEN_MASK)
#define SEMC_DCCR_SRAMXVAL_MASK (0x3E000000U)
#define SEMC_DCCR_SRAMXVAL_SHIFT (25U)
#define SEMC_DCCR_SRAMXVAL(x) (((uint32_t)(((uint32_t)(x)) << SEMC_DCCR_SRAMXVAL_SHIFT)) & SEMC_DCCR_SRAMXVAL_MASK)

/* SDRAM IP command codes (fsl_semc.h semc_ipcmd_sdram_t) + magic key */
#define kSEMC_SDRAMCM_Read 0x8u
#define kSEMC_SDRAMCM_Write 0x9u
#define kSEMC_SDRAMCM_Modeset 0xAu
#define kSEMC_SDRAMCM_Active 0xBu
#define kSEMC_SDRAMCM_AutoRefresh 0xCu
#define kSEMC_SDRAMCM_SelfRefresh 0xDu
#define kSEMC_SDRAMCM_Precharge 0xEu
#define kSEMC_SDRAMCM_Prechargeall 0xFu
#define SEMC_IPCMD_KEY_MAGIC 0xA55Au  /* SEMC_IPCOMMANDMAGICKEY */
#define SEMC_SDRAM_MODESETCAL_OFFSET 4u  /* mode = burstLen | (CL << 4) */

/* SEMC SDRAM pads (SDK semc/sdram pin_mux.c): all IOMUXC MUX_MODE=ALT0,
 * pad-ctl 0x08 = PDRV high-drive + internal pulldown (incl. CLK/CLKX). */
#define SEMC_PAD_MUX_ALT 0x0u
#define SEMC_PAD_CTL_VAL 0x08u
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_00 (*(volatile uint32_t *)0x400E8010u) /* SEMC_DATA00 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_01 (*(volatile uint32_t *)0x400E8014u) /* SEMC_DATA01 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_02 (*(volatile uint32_t *)0x400E8018u) /* SEMC_DATA02 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_03 (*(volatile uint32_t *)0x400E801Cu) /* SEMC_DATA03 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_04 (*(volatile uint32_t *)0x400E8020u) /* SEMC_DATA04 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_05 (*(volatile uint32_t *)0x400E8024u) /* SEMC_DATA05 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_06 (*(volatile uint32_t *)0x400E8028u) /* SEMC_DATA06 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_07 (*(volatile uint32_t *)0x400E802Cu) /* SEMC_DATA07 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_08 (*(volatile uint32_t *)0x400E8030u) /* SEMC_DM00 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_09 (*(volatile uint32_t *)0x400E8034u) /* SEMC_ADDR00 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_10 (*(volatile uint32_t *)0x400E8038u) /* SEMC_ADDR01 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_11 (*(volatile uint32_t *)0x400E803Cu) /* SEMC_ADDR02 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_12 (*(volatile uint32_t *)0x400E8040u) /* SEMC_ADDR03 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_13 (*(volatile uint32_t *)0x400E8044u) /* SEMC_ADDR04 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_14 (*(volatile uint32_t *)0x400E8048u) /* SEMC_ADDR05 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_15 (*(volatile uint32_t *)0x400E804Cu) /* SEMC_ADDR06 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_16 (*(volatile uint32_t *)0x400E8050u) /* SEMC_ADDR07 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_17 (*(volatile uint32_t *)0x400E8054u) /* SEMC_ADDR08 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_18 (*(volatile uint32_t *)0x400E8058u) /* SEMC_ADDR09 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_19 (*(volatile uint32_t *)0x400E805Cu) /* SEMC_ADDR11 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_20 (*(volatile uint32_t *)0x400E8060u) /* SEMC_ADDR12 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_21 (*(volatile uint32_t *)0x400E8064u) /* SEMC_BA0 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_22 (*(volatile uint32_t *)0x400E8068u) /* SEMC_BA1 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_23 (*(volatile uint32_t *)0x400E806Cu) /* SEMC_ADDR10 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_24 (*(volatile uint32_t *)0x400E8070u) /* SEMC_CAS */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_25 (*(volatile uint32_t *)0x400E8074u) /* SEMC_RAS */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_26 (*(volatile uint32_t *)0x400E8078u) /* SEMC_CLK */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_27 (*(volatile uint32_t *)0x400E807Cu) /* SEMC_CKE */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_28 (*(volatile uint32_t *)0x400E8080u) /* SEMC_WE */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_29 (*(volatile uint32_t *)0x400E8084u) /* SEMC_CS0 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_30 (*(volatile uint32_t *)0x400E8088u) /* SEMC_DATA08 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_31 (*(volatile uint32_t *)0x400E808Cu) /* SEMC_DATA09 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_32 (*(volatile uint32_t *)0x400E8090u) /* SEMC_DATA10 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_33 (*(volatile uint32_t *)0x400E8094u) /* SEMC_DATA11 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_34 (*(volatile uint32_t *)0x400E8098u) /* SEMC_DATA12 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_35 (*(volatile uint32_t *)0x400E809Cu) /* SEMC_DATA13 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_36 (*(volatile uint32_t *)0x400E80A0u) /* SEMC_DATA14 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_37 (*(volatile uint32_t *)0x400E80A4u) /* SEMC_DATA15 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_38 (*(volatile uint32_t *)0x400E80A8u) /* SEMC_DM01 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_39 (*(volatile uint32_t *)0x400E80ACu) /* SEMC_DQS */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_40 (*(volatile uint32_t *)0x400E80B0u) /* SEMC_RDY */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_41 (*(volatile uint32_t *)0x400E80B4u) /* SEMC_CSX00 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_00 (*(volatile uint32_t *)0x400E80B8u) /* SEMC_DATA16 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_01 (*(volatile uint32_t *)0x400E80BCu) /* SEMC_DATA17 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_02 (*(volatile uint32_t *)0x400E80C0u) /* SEMC_DATA18 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_03 (*(volatile uint32_t *)0x400E80C4u) /* SEMC_DATA19 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_04 (*(volatile uint32_t *)0x400E80C8u) /* SEMC_DATA20 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_05 (*(volatile uint32_t *)0x400E80CCu) /* SEMC_DATA21 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_06 (*(volatile uint32_t *)0x400E80D0u) /* SEMC_DATA22 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_07 (*(volatile uint32_t *)0x400E80D4u) /* SEMC_DATA23 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_08 (*(volatile uint32_t *)0x400E80D8u) /* SEMC_DM02 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_09 (*(volatile uint32_t *)0x400E80DCu) /* SEMC_DATA24 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_10 (*(volatile uint32_t *)0x400E80E0u) /* SEMC_DATA25 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_11 (*(volatile uint32_t *)0x400E80E4u) /* SEMC_DATA26 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_12 (*(volatile uint32_t *)0x400E80E8u) /* SEMC_DATA27 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_13 (*(volatile uint32_t *)0x400E80ECu) /* SEMC_DATA28 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_14 (*(volatile uint32_t *)0x400E80F0u) /* SEMC_DATA29 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_15 (*(volatile uint32_t *)0x400E80F4u) /* SEMC_DATA30 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_16 (*(volatile uint32_t *)0x400E80F8u) /* SEMC_DATA31 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_17 (*(volatile uint32_t *)0x400E80FCu) /* SEMC_DM03 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_18 (*(volatile uint32_t *)0x400E8100u) /* SEMC_DQS4 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_19 (*(volatile uint32_t *)0x400E8104u) /* SEMC_CLKX00 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B2_20 (*(volatile uint32_t *)0x400E8108u) /* SEMC_CLKX01 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_00 (*(volatile uint32_t *)0x400E8254u) /* SEMC_DATA00 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_01 (*(volatile uint32_t *)0x400E8258u) /* SEMC_DATA01 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_02 (*(volatile uint32_t *)0x400E825Cu) /* SEMC_DATA02 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_03 (*(volatile uint32_t *)0x400E8260u) /* SEMC_DATA03 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_04 (*(volatile uint32_t *)0x400E8264u) /* SEMC_DATA04 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_05 (*(volatile uint32_t *)0x400E8268u) /* SEMC_DATA05 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_06 (*(volatile uint32_t *)0x400E826Cu) /* SEMC_DATA06 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_07 (*(volatile uint32_t *)0x400E8270u) /* SEMC_DATA07 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_08 (*(volatile uint32_t *)0x400E8274u) /* SEMC_DM00 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_09 (*(volatile uint32_t *)0x400E8278u) /* SEMC_ADDR00 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_10 (*(volatile uint32_t *)0x400E827Cu) /* SEMC_ADDR01 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_11 (*(volatile uint32_t *)0x400E8280u) /* SEMC_ADDR02 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_12 (*(volatile uint32_t *)0x400E8284u) /* SEMC_ADDR03 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_13 (*(volatile uint32_t *)0x400E8288u) /* SEMC_ADDR04 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_14 (*(volatile uint32_t *)0x400E828Cu) /* SEMC_ADDR05 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_15 (*(volatile uint32_t *)0x400E8290u) /* SEMC_ADDR06 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_16 (*(volatile uint32_t *)0x400E8294u) /* SEMC_ADDR07 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_17 (*(volatile uint32_t *)0x400E8298u) /* SEMC_ADDR08 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_18 (*(volatile uint32_t *)0x400E829Cu) /* SEMC_ADDR09 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_19 (*(volatile uint32_t *)0x400E82A0u) /* SEMC_ADDR11 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_20 (*(volatile uint32_t *)0x400E82A4u) /* SEMC_ADDR12 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_21 (*(volatile uint32_t *)0x400E82A8u) /* SEMC_BA0 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_22 (*(volatile uint32_t *)0x400E82ACu) /* SEMC_BA1 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_23 (*(volatile uint32_t *)0x400E82B0u) /* SEMC_ADDR10 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_24 (*(volatile uint32_t *)0x400E82B4u) /* SEMC_CAS */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_25 (*(volatile uint32_t *)0x400E82B8u) /* SEMC_RAS */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_26 (*(volatile uint32_t *)0x400E82BCu) /* SEMC_CLK */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_27 (*(volatile uint32_t *)0x400E82C0u) /* SEMC_CKE */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_28 (*(volatile uint32_t *)0x400E82C4u) /* SEMC_WE */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_29 (*(volatile uint32_t *)0x400E82C8u) /* SEMC_CS0 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_30 (*(volatile uint32_t *)0x400E82CCu) /* SEMC_DATA08 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_31 (*(volatile uint32_t *)0x400E82D0u) /* SEMC_DATA09 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_32 (*(volatile uint32_t *)0x400E82D4u) /* SEMC_DATA10 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_33 (*(volatile uint32_t *)0x400E82D8u) /* SEMC_DATA11 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_34 (*(volatile uint32_t *)0x400E82DCu) /* SEMC_DATA12 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_35 (*(volatile uint32_t *)0x400E82E0u) /* SEMC_DATA13 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_36 (*(volatile uint32_t *)0x400E82E4u) /* SEMC_DATA14 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_37 (*(volatile uint32_t *)0x400E82E8u) /* SEMC_DATA15 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_38 (*(volatile uint32_t *)0x400E82ECu) /* SEMC_DM01 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_39 (*(volatile uint32_t *)0x400E82F0u) /* SEMC_DQS */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_40 (*(volatile uint32_t *)0x400E82F4u) /* SEMC_RDY */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_41 (*(volatile uint32_t *)0x400E82F8u) /* SEMC_CSX00 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_00 (*(volatile uint32_t *)0x400E82FCu) /* SEMC_DATA16 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_01 (*(volatile uint32_t *)0x400E8300u) /* SEMC_DATA17 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_02 (*(volatile uint32_t *)0x400E8304u) /* SEMC_DATA18 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_03 (*(volatile uint32_t *)0x400E8308u) /* SEMC_DATA19 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_04 (*(volatile uint32_t *)0x400E830Cu) /* SEMC_DATA20 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_05 (*(volatile uint32_t *)0x400E8310u) /* SEMC_DATA21 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_06 (*(volatile uint32_t *)0x400E8314u) /* SEMC_DATA22 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_07 (*(volatile uint32_t *)0x400E8318u) /* SEMC_DATA23 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_08 (*(volatile uint32_t *)0x400E831Cu) /* SEMC_DM02 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_09 (*(volatile uint32_t *)0x400E8320u) /* SEMC_DATA24 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_10 (*(volatile uint32_t *)0x400E8324u) /* SEMC_DATA25 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_11 (*(volatile uint32_t *)0x400E8328u) /* SEMC_DATA26 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_12 (*(volatile uint32_t *)0x400E832Cu) /* SEMC_DATA27 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_13 (*(volatile uint32_t *)0x400E8330u) /* SEMC_DATA28 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_14 (*(volatile uint32_t *)0x400E8334u) /* SEMC_DATA29 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_15 (*(volatile uint32_t *)0x400E8338u) /* SEMC_DATA30 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_16 (*(volatile uint32_t *)0x400E833Cu) /* SEMC_DATA31 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_17 (*(volatile uint32_t *)0x400E8340u) /* SEMC_DM03 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_18 (*(volatile uint32_t *)0x400E8344u) /* SEMC_DQS4 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_19 (*(volatile uint32_t *)0x400E8348u) /* SEMC_CLKX00 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B2_20 (*(volatile uint32_t *)0x400E834Cu) /* SEMC_CLKX01 */

/* SEMC clock: root 4 (kCLOCK_Root_Semc) = SYS_PLL2_PFD1 594MHz / 3 = 198MHz;
 * gate LPCG33 (kCLOCK_Semc). semc_clock_hz = 198000000 for Task 2 timing. */
#define CCM_CLOCK_ROOT4_CONTROL (*(volatile uint32_t *)0x40CC0200u) /* kCLOCK_Root_Semc */
#define CCM_LPCG33_DIRECT       (*(volatile uint32_t *)0x40CC6420u) /* kCLOCK_Semc gate */
#define SEMC_CLOCK_HZ 198000000u  /* SYS_PLL2_PFD1 (594 MHz) / 3 */

/* External SDRAM attribute -> .externalram (imxrt1176.ld ERAM @ 0x80000000) */
#ifndef EXTMEM
#define EXTMEM __attribute__((section(".externalram"), used))
#endif

/* ==== ENET 10/100 (Freescale FEC) @ 0x40424000, IRQ 137 ==== */
/* Register offsets verified against RT1170/periph/PERI_ENET.h */
#define ENET_EIR   (*(volatile uint32_t *)(ENET_BASE + 0x004u))
#define ENET_EIMR  (*(volatile uint32_t *)(ENET_BASE + 0x008u))
#define ENET_RDAR  (*(volatile uint32_t *)(ENET_BASE + 0x010u))
#define ENET_TDAR  (*(volatile uint32_t *)(ENET_BASE + 0x014u))
#define ENET_ECR   (*(volatile uint32_t *)(ENET_BASE + 0x024u))
#define ENET_MMFR  (*(volatile uint32_t *)(ENET_BASE + 0x040u))
#define ENET_MSCR  (*(volatile uint32_t *)(ENET_BASE + 0x044u))
#define ENET_MIBC  (*(volatile uint32_t *)(ENET_BASE + 0x064u))
#define ENET_RCR   (*(volatile uint32_t *)(ENET_BASE + 0x084u))
#define ENET_TCR   (*(volatile uint32_t *)(ENET_BASE + 0x0C4u))
#define ENET_PALR  (*(volatile uint32_t *)(ENET_BASE + 0x0E4u))
#define ENET_PAUR  (*(volatile uint32_t *)(ENET_BASE + 0x0E8u))
#define ENET_OPD   (*(volatile uint32_t *)(ENET_BASE + 0x0ECu))
#define ENET_TFWR  (*(volatile uint32_t *)(ENET_BASE + 0x144u))
#define ENET_RDSR  (*(volatile uint32_t *)(ENET_BASE + 0x180u))
#define ENET_TDSR  (*(volatile uint32_t *)(ENET_BASE + 0x184u))
#define ENET_MRBR  (*(volatile uint32_t *)(ENET_BASE + 0x188u))
#define ENET_TACC  (*(volatile uint32_t *)(ENET_BASE + 0x1C0u))
#define ENET_RACC  (*(volatile uint32_t *)(ENET_BASE + 0x1C4u))
/* ECR bits (verified: RESET=bit0, ETHEREN=bit1, EN1588=bit4, DBSWP=bit8) */
#define ENET_ECR_RESET   (1u<<0)
#define ENET_ECR_ETHEREN (1u<<1)
#define ENET_ECR_EN1588  (1u<<4)
#define ENET_ECR_DBSWP   (1u<<8)
/* EIR/EIMR bits (verified: MII=bit23, RXF=bit25, TXF=bit27) */
#define ENET_EIR_MII (1u<<23)
#define ENET_EIR_RXF (1u<<25)
#define ENET_EIR_TXF (1u<<27)
/* RDAR/TDAR active bit (SDK names the field RDAR/TDAR; mask 0x1000000=bit24) */
#define ENET_RDAR_ACTIVE (1u<<24)
#define ENET_TDAR_ACTIVE (1u<<24)
/* MSCR MII_SPEED field (verified mask 0x7E, shift 1 -> 6-bit field): MDC = ref/((MII_SPEED+1)*2) */
#define ENET_MSCR_MII_SPEED(n) (((n)&0x3Fu)<<1)
/* RCR bits (RT1176 RM / FNET fnet_fec.h; QEMU imx_fec.h): MII_MODE=bit2,
 * RMII_MODE=bit8, DRT(half-duplex)=bit1, FCE=bit5, MAX_FL=[29:16] (14-bit). */
#define ENET_RCR_DRT       (1u<<1)
#define ENET_RCR_MII_MODE  (1u<<2)
#define ENET_RCR_FCE       (1u<<5)
#define ENET_RCR_RMII_MODE (1u<<8)
#define ENET_RCR_MAX_FL(n) (((n)&0x3FFFu)<<16)
/* TCR bits: GTS=bit0, FDEN(full-duplex)=bit2 */
#define ENET_TCR_GTS       (1u<<0)
#define ENET_TCR_FDEN      (1u<<2)
/* MMFR fields (clause-22; verified against ENET_MMFR_ST/OP/TA/PA/RA masks) */
#define ENET_MMFR_ST_01   (1u<<30)
#define ENET_MMFR_OP_READ (2u<<28)
#define ENET_MMFR_OP_WRITE (1u<<28)
#define ENET_MMFR_TA_10   (2u<<16)
#define ENET_MMFR_PA(a)   (((a)&0x1Fu)<<23)
#define ENET_MMFR_RA(r)   (((r)&0x1Fu)<<18)
/* Legacy 8-byte buffer-descriptor status bits: these live in the BD memory
 * structure, not the MMIO register map, so PERI_ENET.h has no masks for
 * them -- standard Freescale/NXP FEC BD layout (same across i.MX RT/
 * Kinetis/Vybrid), per the RT1170 RM ENET chapter. */
#define ENET_TXBD_R  (1u<<15)
#define ENET_TXBD_TO1 (1u<<14)
#define ENET_TXBD_W  (1u<<13)
#define ENET_TXBD_TO2 (1u<<12)
#define ENET_TXBD_L  (1u<<11)
#define ENET_TXBD_TC (1u<<10)
#define ENET_RXBD_E  (1u<<15)
#define ENET_RXBD_W  (1u<<13)
#define ENET_RXBD_L  (1u<<11)
/* RX BD error/status bits (RM legacy RxBD; match FNET + QEMU imx_fec.h) */
#define ENET_RXBD_M  (1u<<8)   /* MISS (promiscuous)          */
#define ENET_RXBD_BC (1u<<7)   /* broadcast                   */
#define ENET_RXBD_MC (1u<<6)   /* multicast                   */
#define ENET_RXBD_LG (1u<<5)   /* frame length violation      */
#define ENET_RXBD_NO (1u<<4)   /* non-octet aligned frame     */
#define ENET_RXBD_CR (1u<<2)   /* CRC/frame error             */
#define ENET_RXBD_OV (1u<<1)   /* FIFO overrun                */
#define ENET_RXBD_TR (1u<<0)   /* frame truncated             */
#define ENET_RXBD_ERR (ENET_RXBD_LG|ENET_RXBD_NO|ENET_RXBD_CR|ENET_RXBD_OV|ENET_RXBD_TR)
/* IOMUXC_GPR ENET fields.  GPR4 offset 0x10, GPR28 offset 0x70 (both
 * verified against the PERI_IOMUXC_GPR.h IOMUXC_GPR_Type struct); absolute-
 * address style matches the existing GPR0/14/16/17/18 defines above (this
 * codebase does not route GPR regs through IOMUXC_GPR_BASE even though that
 * macro exists).  Neither GPR4 nor GPR28 was previously defined. */
#define IOMUXC_GPR_GPR4  (*(volatile uint32_t *)0x400E4010u)
#define IOMUXC_GPR_GPR4_ENET_REF_CLK_DIR (1u<<1)
#define IOMUXC_GPR_GPR28 (*(volatile uint32_t *)0x400E4070u)
#define IOMUXC_GPR_GPR28_CACHE_ENET (1u<<7)
/* ENET clock: root 51 = kCLOCK_Root_Enet1, LPCG gate 112 = kCLOCK_Enet
 * (both confirmed in fsl_clock.h clock_root_t/clock_lpcg_t).  mux=4
 * (SysPll1Div2), div=10 -> 50 MHz matches the SDK enet/txrx_transfer
 * BOARD_InitModuleClock {.mux=4,.div=10}.  Root/LPCG addresses follow the
 * generator's own CCM strides (ROOT n @ 0x40CC0000 + n*0x80; LPCG n DIRECT
 * @ 0x40CC6000 + n*0x20), cross-checked vs ROOT58/LPCG117 above. */
#define CCM_CLOCK_ROOT51_CONTROL (*(volatile uint32_t *)0x40CC1980u)
#define CCM_LPCG112_DIRECT       (*(volatile uint32_t *)0x40CC6E00u)
/* ENET RMII + MDIO + PHY-reset pads (Task 3) -- verified vs fsl_iomuxc.h.
 * DISP_B2/AD pads in the main IOMUXC block @ 0x400E8000; LPSR_12 in the
 * LPSR block @ 0x40C08000.  ENET_*_SELECT_INPUT daisy regs are one-offs,
 * written to literal addresses directly in enet.c (not emitted here). */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_32       (*(volatile uint32_t *)0x400E818Cu) /* ENET_MDC   ALT3 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_33       (*(volatile uint32_t *)0x400E8190u) /* ENET_MDIO  ALT3 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_02  (*(volatile uint32_t *)0x400E821Cu) /* ENET_TX_DATA00 ALT1 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_03  (*(volatile uint32_t *)0x400E8220u) /* ENET_TX_DATA01 ALT1 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_04  (*(volatile uint32_t *)0x400E8224u) /* ENET_TX_EN     ALT1 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_05  (*(volatile uint32_t *)0x400E8228u) /* ENET_REF_CLK   ALT2 (SION) */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_06  (*(volatile uint32_t *)0x400E822Cu) /* ENET_RX_DATA00 ALT1 (SION) */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_07  (*(volatile uint32_t *)0x400E8230u) /* ENET_RX_DATA01 ALT1 (SION) */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_08  (*(volatile uint32_t *)0x400E8234u) /* ENET_RX_EN     ALT1 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_09  (*(volatile uint32_t *)0x400E8238u) /* ENET_RX_ER     ALT1 */
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_LPSR_12     (*(volatile uint32_t *)0x40C08030u) /* GPIO12_IO12 ALT0xA (PHY reset) */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_32       (*(volatile uint32_t *)0x400E83D0u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_33       (*(volatile uint32_t *)0x400E83D4u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_02  (*(volatile uint32_t *)0x400E8460u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_03  (*(volatile uint32_t *)0x400E8464u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_04  (*(volatile uint32_t *)0x400E8468u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_05  (*(volatile uint32_t *)0x400E846Cu)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_06  (*(volatile uint32_t *)0x400E8470u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_07  (*(volatile uint32_t *)0x400E8474u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_08  (*(volatile uint32_t *)0x400E8478u)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_09  (*(volatile uint32_t *)0x400E847Cu)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_LPSR_12     (*(volatile uint32_t *)0x40C08070u)

/* --- SNVS RTC (Secure Non-Volatile Storage: HP RTC + LP secure RTC) --- */
#define SNVS_HPLR      (*(volatile uint32_t *)0x40C90000u)
#define SNVS_HPCOMR    (*(volatile uint32_t *)0x40C90004u)
#define SNVS_HPCR      (*(volatile uint32_t *)0x40C90008u)
#define SNVS_HPSR      (*(volatile uint32_t *)0x40C90014u)
#define SNVS_HPRTCMR   (*(volatile uint32_t *)0x40C90024u)
#define SNVS_HPRTCLR   (*(volatile uint32_t *)0x40C90028u)
#define SNVS_HPTAMR    (*(volatile uint32_t *)0x40C9002Cu)
#define SNVS_HPTALR    (*(volatile uint32_t *)0x40C90030u)
#define SNVS_LPLR      (*(volatile uint32_t *)0x40C90034u)
#define SNVS_LPCR      (*(volatile uint32_t *)0x40C90038u)
#define SNVS_LPSR      (*(volatile uint32_t *)0x40C9004Cu)
#define SNVS_LPSRTCMR  (*(volatile uint32_t *)0x40C90050u)
#define SNVS_LPSRTCLR  (*(volatile uint32_t *)0x40C90054u)
#define SNVS_LPGPR0    (*(volatile uint32_t *)0x40C90100u)
#define SNVS_LPGPR1    (*(volatile uint32_t *)0x40C90104u)
#define SNVS_LPGPR2    (*(volatile uint32_t *)0x40C90108u)
#define SNVS_LPGPR3    (*(volatile uint32_t *)0x40C9010Cu)

/* SNVS bitfields (RT1176 RM / PERI_SNVS.h) */
#define SNVS_HPCR_RTC_EN     (1u << 0)   /* HP RTC enable            */
#define SNVS_HPCR_HP_TS      (1u << 16)  /* HP time-sync: load HP<-LP */
#define SNVS_LPCR_SRTC_ENV   (1u << 0)   /* LP secure RTC valid/enable */

/* --- DAC12 (12-bit DAC, base 0x40064000, IRQ 63, LPCG 57; Task: analogWriteDAC0) ---
 * PERI_DAC.h / RM ch.89. RT1170 chip-specifics (RM 89.1): VREFH is the ONLY
 * reference (CR.DACRFS must be 1), no HW trigger, no ITRM register, IREF2/PTAT
 * current ref unavailable. DATA is write-only on silicon. */
#define DAC_VERID (*(volatile uint32_t *)0x40064000u)
#define DAC_PARAM (*(volatile uint32_t *)0x40064004u)   /* FIFOSZ[2:0]=0b011 -> 16-word FIFO */
#define DAC_DATA  (*(volatile uint32_t *)0x40064008u)   /* write-only: DATA0[11:0] */
#define DAC_CR    (*(volatile uint32_t *)0x4006400Cu)
#define DAC_PTR   (*(volatile uint32_t *)0x40064010u)
#define DAC_CR2   (*(volatile uint32_t *)0x40064014u)
#define DAC_PARAM_FIFOSZ_MASK   0x7u
#define DAC_DATA_DATA0_MASK     0xFFFu
#define DAC_CR_FULLF_MASK       0x1u        /* ro: FIFO full */
#define DAC_CR_NEMPTF_MASK      0x2u        /* ro: one datum left */
#define DAC_CR_WMF_MASK         0x4u        /* ro: remaining < WML */
#define DAC_CR_UDFF_MASK        0x8u        /* w1c: trigger underflow */
#define DAC_CR_OVFF_MASK        0x10u       /* w1c: FIFO overflow */
#define DAC_CR_FULLIE_MASK      0x100u
#define DAC_CR_EMPTIE_MASK      0x200u
#define DAC_CR_WTMIE_MASK       0x400u
#define DAC_CR_SWTRG_MASK       0x1000u     /* self-clearing */
#define DAC_CR_TRGSEL_MASK      0x2000u     /* 1 = SW trigger (only option on RT1170) */
#define DAC_CR_DACRFS_MASK      0x4000u     /* MUST be 1 on RT1170 (VREFH) */
#define DAC_CR_DACEN_MASK       0x8000u
#define DAC_CR_FIFOEN_MASK      0x10000u
#define DAC_CR_SWMD_MASK        0x20000u
#define DAC_CR_UVIE_MASK        0x40000u
#define DAC_CR_FIFORST_MASK     0x200000u   /* self-clearing */
#define DAC_CR_SWRST_MASK       0x400000u   /* self-clearing */
#define DAC_CR_DMAEN_MASK       0x800000u
#define DAC_CR_WML_SHIFT        24
#define DAC_CR_WML_MASK         0xFF000000u
#define DAC_PTR_DACWFP_MASK     0xFFu
#define DAC_PTR_DACRFP_SHIFT    16
#define DAC_PTR_DACRFP_MASK     0xFF0000u
#define DAC_CR2_BFEN_MASK       0x1u        /* opamp buffer */
#define DAC_CR2_OEN_MASK        0x2u        /* buffer bypassed */
#define DAC_CR2_BFMS_MASK       0x4u
#define DAC_CR2_BFHS_MASK       0x8u
#define DAC_CR2_IREF1_MASK      0x20u       /* ZTC current ref */
#define DAC_CR2_IREF_MASK       0x40u       /* internal current ref (SDK default) */
/* DAC clock gate: kCLOCK_Dac = LPCG 57 (0x40CC6000 + 57*0x20). */
#define CCM_LPCG57_DIRECT (*(volatile uint32_t *)0x40CC6720u)

/* --- PXP (Pixel Pipeline) 2D graphics accelerator --------------------------
 * RM rev.5 ch.52.  Base 0x40814000, IRQ 57, clock gate LPCG127 (BUS_CLK_ROOT).
 * NOTE: CTRL resets to 0xC000_0000 - SFTRST and CLKGATE are BOTH SET, so the
 * block is held dead until the RM 52.5 sequence runs (set SFTRST; then clear
 * SFTRST and CLKGATE).  Registers at +0x4/+0x8/+0xC are MXS SET/CLR/TOG
 * aliases of the base register. */
#define PXP_BASE                  0x40814000u
#define PXP_REG(off)              (*(volatile uint32_t *)(PXP_BASE + (off)))

#define PXP_CTRL                  PXP_REG(0x000)
#define PXP_CTRL_SET              PXP_REG(0x004)
#define PXP_CTRL_CLR              PXP_REG(0x008)
#define PXP_CTRL_TOG              PXP_REG(0x00C)
#define PXP_STAT                  PXP_REG(0x010)
#define PXP_STAT_SET              PXP_REG(0x014)
#define PXP_STAT_CLR              PXP_REG(0x018)
#define PXP_STAT_TOG              PXP_REG(0x01C)
#define PXP_OUT_CTRL              PXP_REG(0x020)
#define PXP_OUT_CTRL_SET          PXP_REG(0x024)
#define PXP_OUT_CTRL_CLR          PXP_REG(0x028)
#define PXP_OUT_CTRL_TOG          PXP_REG(0x02C)
#define PXP_OUT_BUF               PXP_REG(0x030)
#define PXP_OUT_BUF2              PXP_REG(0x040)
#define PXP_OUT_PITCH             PXP_REG(0x050)
#define PXP_OUT_LRC               PXP_REG(0x060)
#define PXP_OUT_PS_ULC            PXP_REG(0x070)
#define PXP_OUT_PS_LRC            PXP_REG(0x080)
#define PXP_OUT_AS_ULC            PXP_REG(0x090)
#define PXP_OUT_AS_LRC            PXP_REG(0x0A0)
#define PXP_PS_CTRL               PXP_REG(0x0B0)
#define PXP_PS_CTRL_SET           PXP_REG(0x0B4)
#define PXP_PS_CTRL_CLR           PXP_REG(0x0B8)
#define PXP_PS_CTRL_TOG           PXP_REG(0x0BC)
#define PXP_PS_BUF                PXP_REG(0x0C0)
#define PXP_PS_UBUF               PXP_REG(0x0D0)
#define PXP_PS_VBUF               PXP_REG(0x0E0)
#define PXP_PS_PITCH              PXP_REG(0x0F0)
#define PXP_PS_BACKGROUND         PXP_REG(0x100)
#define PXP_PS_SCALE              PXP_REG(0x110)
#define PXP_PS_OFFSET             PXP_REG(0x120)
#define PXP_PS_CLRKEYLOW          PXP_REG(0x130)
#define PXP_PS_CLRKEYHIGH         PXP_REG(0x140)
#define PXP_AS_CTRL               PXP_REG(0x150)
#define PXP_AS_BUF                PXP_REG(0x160)
#define PXP_AS_PITCH              PXP_REG(0x170)
#define PXP_AS_CLRKEYLOW          PXP_REG(0x180)
#define PXP_AS_CLRKEYHIGH         PXP_REG(0x190)
#define PXP_CSC1_COEF0            PXP_REG(0x1A0)
#define PXP_CSC1_COEF1            PXP_REG(0x1B0)
#define PXP_CSC1_COEF2            PXP_REG(0x1C0)
/* CSC1_COEF0 bits.  BYPASS resets to 0, so the PS datapath runs every source
 * pixel through the YUV->RGB colour matrix by default - it MUST be set for an
 * RGB source, or the copy is colour-mangled (HW-confirmed; QEMU does not model
 * CSC1). */
#define PXP_CSC1_BYPASS          ((uint32_t)(1u<<30))
#define PXP_CSC1_YCBCR_MODE      ((uint32_t)(1u<<31))
#define PXP_POWER                 PXP_REG(0x320)
#define PXP_NEXT                  PXP_REG(0x400)
#define PXP_PORTER_DUFF_CTRL      PXP_REG(0x440)

/* CTRL bits - bitmasks, not registers (PXP_CTRL_SET/_CLR/_TOG above are the
 * registers; same PXP_CTRL_ prefix, NXP MXS style) - OR into PXP_CTRL, or
 * write one alone into PXP_CTRL_SET/_CLR for an atomic set/clear. */
#define PXP_CTRL_ENABLE           ((uint32_t)(1u<<0))
#define PXP_CTRL_IRQ_ENABLE       ((uint32_t)(1u<<1))
#define PXP_CTRL_NEXT_IRQ_ENABLE  ((uint32_t)(1u<<2))
#define PXP_CTRL_ROTATE_SHIFT     8
#define PXP_CTRL_ROTATE_MASK      ((uint32_t)(3u<<8))
#define PXP_CTRL_HFLIP            ((uint32_t)(1u<<10))
#define PXP_CTRL_VFLIP            ((uint32_t)(1u<<11))
#define PXP_CTRL_ROT_POS          ((uint32_t)(1u<<22))  /* 0=rotate after composite */
#define PXP_CTRL_BLOCK_SIZE       ((uint32_t)(1u<<23))  /* 0=8x8, 1=16x16 blocks   */
#define PXP_CTRL_CLKGATE          ((uint32_t)(1u<<30))
#define PXP_CTRL_SFTRST           ((uint32_t)(1u<<31))

/* STAT bits - all confirmed against RM 52.6.2 (see Step 1). Bitmasks, not
 * registers (PXP_STAT_SET/_CLR/_TOG above are the registers; same
 * PXP_STAT_ prefix, NXP MXS style) - OR into PXP_STAT, or write one alone
 * into PXP_STAT_SET/_CLR for an atomic set/clear.
 * Bit 8 (LUT_DMA_LOAD_DONE_IRQ) is deliberately NOT defined here: it is a
 * doc artefact carried over from i.MX6 - this PXP has no LUT registers. */
#define PXP_STAT_IRQ              ((uint32_t)(1u<<0))
#define PXP_STAT_AXI_WRITE_ERROR  ((uint32_t)(1u<<1))
#define PXP_STAT_AXI_READ_ERROR   ((uint32_t)(1u<<2))
#define PXP_STAT_NEXT_IRQ         ((uint32_t)(1u<<3))

/* FORMAT fields - three DIFFERENT widths, deliberately not one shared mask.
 * The library translates an abstract PXPFormat into the right encoding per
 * role (pxpPsFormat/pxpOutFormat); these masks only bound the field.
 * PS_CTRL/OUT_CTRL's FORMAT field starts at bit 0, so the mask can be OR'd
 * in directly with no shift; AS_CTRL's FORMAT sits at [7:4] instead, hence
 * the one PXP_AS_FORMAT_SHIFT below (PS/OUT have no shift to name). */
#define PXP_PS_FORMAT_MASK        ((uint32_t)0x3Fu)   /* PS_CTRL  [5:0] */
#define PXP_OUT_FORMAT_MASK       ((uint32_t)0x1Fu)   /* OUT_CTRL [4:0] */
#define PXP_AS_FORMAT_MASK        ((uint32_t)0x0Fu)   /* AS_CTRL  [7:4] */
#define PXP_AS_FORMAT_SHIFT       4

/* Coordinate registers: X in [29:16], Y in [13:0]. */
#define PXP_COORD(x, y) \
    ((((uint32_t)(x) & 0x3FFFu) << 16) | ((uint32_t)(y) & 0x3FFFu))

/* PXP clock gate: LPCG127 = 0x40CC6000 + 127*0x20 */
#define CCM_LPCG127_DIRECT        (*(volatile uint32_t *)0x40CC6FE0u)

/* ===========================================================================
 * DISPLAY: LCDIFv2 display controller + MIPI DSI host (RPi 7" MIPI-DSI panel).
 * Register offsets/bitmasks transcribed (facts only, re-expressed in this
 * file's #define idiom -- NXP struct typedefs NOT copied) from the NXP
 * MCUXpresso SDK (BSD-3-Clause) peripheral-access headers for MIMXRT1176:
 *   PERI_LCDIFV2.h, PERI_DSI_HOST.h, PERI_DSI_HOST_DPI_INTFC.h,
 *   PERI_DSI_HOST_APB_PKT_IF.h, PERI_DSI_HOST_NXP_FDSOI28_DPHY_INTFC.h.
 * Instance/sub-block bases + IRQs from MIMXRT1176_cm7_COMMON.h; clock-root and
 * LPCG-gate indices from the SDK fsl_clock.h clock_root_t/clock_lpcg_t enums.
 * =========================================================================== */

/* --- LCDIFv2 display controller --------------------------------------------
 * LCDIFv2 @ 0x40808000, IRQ 55 (LCDIFv2_IRQn), clock gate LPCG130.
 * Full register file per PERI_LCDIFV2.h.  The layer descriptor block is an
 * 8-entry array LAYER[0..7] (array step 0x40 starting at 0x200); v1 scans out
 * RGB565 from LAYER 0.  LCDIFv2 has NXP MXS SET/CLR/TOG aliases ONLY on CTRL
 * (+0x4/+0x8/+0xC) -- no other LCDIFv2 register has them. */
#define LCDIFV2_BASE              0x40808000u
#define LCDIFV2_REG(off)          (*(volatile uint32_t *)(LCDIFV2_BASE + (off)))

#define LCDIFV2_CTRL              LCDIFV2_REG(0x00)   /* display control        */
#define LCDIFV2_CTRL_SET          LCDIFV2_REG(0x04)   /* MXS atomic set alias   */
#define LCDIFV2_CTRL_CLR          LCDIFV2_REG(0x08)   /* MXS atomic clear alias */
#define LCDIFV2_CTRL_TOG          LCDIFV2_REG(0x0C)   /* MXS atomic toggle alias*/
#define LCDIFV2_DISP_PARA         LCDIFV2_REG(0x10)   /* display parameters     */
#define LCDIFV2_DISP_SIZE         LCDIFV2_REG(0x14)   /* active resolution      */
#define LCDIFV2_HSYN_PARA         LCDIFV2_REG(0x18)   /* H front/pulse/back     */
#define LCDIFV2_VSYN_PARA         LCDIFV2_REG(0x1C)   /* V front/pulse/back     */
/* Interrupt status/enable are a 2-entry array INT[domain] (step 0x10):
 * domain 0 @ 0x20/0x24, domain 1 @ 0x30/0x34. */
#define LCDIFV2_INT_STATUS(d)     LCDIFV2_REG(0x20 + (d)*0x10)
#define LCDIFV2_INT_ENABLE(d)     LCDIFV2_REG(0x24 + (d)*0x10)
#define LCDIFV2_INT_STATUS_D0     LCDIFV2_REG(0x20)
#define LCDIFV2_INT_ENABLE_D0     LCDIFV2_REG(0x24)
#define LCDIFV2_INT_STATUS_D1     LCDIFV2_REG(0x30)
#define LCDIFV2_INT_ENABLE_D1     LCDIFV2_REG(0x34)
#define LCDIFV2_PDI_PARA          LCDIFV2_REG(0x40)
/* Layer n descriptor registers: base 0x200 + n*0x40, n in [0..7].  These are
 * the NXP CTRLDESCL1..6 register names within one layer descriptor (NOT a
 * layer index) -- e.g. LCDIFV2_CTRLDESCL1(0) is layer-0 width/height. */
#define LCDIFV2_CTRLDESCL1(n)     LCDIFV2_REG(0x200 + (n)*0x40)  /* WIDTH/HEIGHT */
#define LCDIFV2_CTRLDESCL2(n)     LCDIFV2_REG(0x204 + (n)*0x40)  /* POSX/POSY    */
#define LCDIFV2_CTRLDESCL3(n)     LCDIFV2_REG(0x208 + (n)*0x40)  /* PITCH        */
#define LCDIFV2_CTRLDESCL4(n)     LCDIFV2_REG(0x20C + (n)*0x40)  /* buffer ADDR  */
#define LCDIFV2_CTRLDESCL5(n)     LCDIFV2_REG(0x210 + (n)*0x40)  /* BPP/EN/blend */
#define LCDIFV2_CTRLDESCL6(n)     LCDIFV2_REG(0x214 + (n)*0x40)  /* BCLR bkgnd   */
#define LCDIFV2_CSC_COEF0(n)      LCDIFV2_REG(0x218 + (n)*0x40)
#define LCDIFV2_CSC_COEF1(n)      LCDIFV2_REG(0x21C + (n)*0x40)
#define LCDIFV2_CSC_COEF2(n)      LCDIFV2_REG(0x220 + (n)*0x40)
#define LCDIFV2_CLUT_LOAD         LCDIFV2_REG(0x400)

/* CTRL bits */
#define LCDIFV2_CTRL_INV_HS       ((uint32_t)(1u<<0))   /* invert HSYNC (active low)  */
#define LCDIFV2_CTRL_INV_VS       ((uint32_t)(1u<<1))   /* invert VSYNC (active low)  */
#define LCDIFV2_CTRL_INV_DE       ((uint32_t)(1u<<2))   /* invert data-enable         */
#define LCDIFV2_CTRL_INV_PXCK     ((uint32_t)(1u<<3))   /* sample data on rising edge */
#define LCDIFV2_CTRL_NEG          ((uint32_t)(1u<<4))   /* negate pixel output        */
#define LCDIFV2_CTRL_SW_RESET     ((uint32_t)(1u<<31))
/* DISP_PARA */
#define LCDIFV2_DISP_PARA_BGND_B(x)      (((uint32_t)(x) << 0) & 0x000000FFu)
#define LCDIFV2_DISP_PARA_BGND_G(x)      (((uint32_t)(x) << 8) & 0x0000FF00u)
#define LCDIFV2_DISP_PARA_BGND_R(x)      (((uint32_t)(x) << 16) & 0x00FF0000u)
#define LCDIFV2_DISP_PARA_DISP_MODE(x)   (((uint32_t)(x) << 24) & 0x03000000u) /* 0=normal */
#define LCDIFV2_DISP_PARA_LINE_PATTERN(x)(((uint32_t)(x) << 26) & 0x1C000000u) /* 0=RGB    */
#define LCDIFV2_DISP_PARA_DISP_ON        ((uint32_t)(1u<<31))
/* DISP_SIZE */
#define LCDIFV2_DISP_SIZE_DELTA_X(x)     (((uint32_t)(x) << 0) & 0x00000FFFu)  /* width  */
#define LCDIFV2_DISP_SIZE_DELTA_Y(x)     (((uint32_t)(x) << 16) & 0x0FFF0000u) /* height */
/* HSYN_PARA / VSYN_PARA: FP=front porch, PW=sync pulse width, BP=back porch */
#define LCDIFV2_HSYN_PARA_FP_H(x)        (((uint32_t)(x) << 0) & 0x000001FFu)
#define LCDIFV2_HSYN_PARA_PW_H(x)        (((uint32_t)(x) << 11) & 0x000FF800u)
#define LCDIFV2_HSYN_PARA_BP_H(x)        (((uint32_t)(x) << 22) & 0x7FC00000u)
#define LCDIFV2_VSYN_PARA_FP_V(x)        (((uint32_t)(x) << 0) & 0x000001FFu)
#define LCDIFV2_VSYN_PARA_PW_V(x)        (((uint32_t)(x) << 11) & 0x000FF800u)
#define LCDIFV2_VSYN_PARA_BP_V(x)        (((uint32_t)(x) << 22) & 0x7FC00000u)
/* Same bit positions apply to both INT_STATUS(d) and INT_ENABLE(d). */
#define LCDIFV2_INT_VSYNC         ((uint32_t)(1u<<0))
#define LCDIFV2_INT_UNDERRUN      ((uint32_t)(1u<<1))
#define LCDIFV2_INT_VS_BLANK      ((uint32_t)(1u<<2))
#define LCDIFV2_INT_DMA_ERR_MASK  ((uint32_t)0x0000FF00u)   /* [15:8]  per-plane */
#define LCDIFV2_INT_DMA_DONE_MASK ((uint32_t)0x00FF0000u)   /* [23:16] per-plane */
#define LCDIFV2_INT_FIFO_EMPTY_MASK ((uint32_t)0xFF000000u) /* [31:24] per-plane */
/* CTRLDESCL1: layer WIDTH/HEIGHT in pixels */
#define LCDIFV2_CTRLDESCL1_WIDTH(x)      (((uint32_t)(x) << 0) & 0x00000FFFu)
#define LCDIFV2_CTRLDESCL1_HEIGHT(x)     (((uint32_t)(x) << 16) & 0x0FFF0000u)
/* CTRLDESCL2: layer position */
#define LCDIFV2_CTRLDESCL2_POSX(x)       (((uint32_t)(x) << 0) & 0x00000FFFu)
#define LCDIFV2_CTRLDESCL2_POSY(x)       (((uint32_t)(x) << 16) & 0x0FFF0000u)
/* CTRLDESCL3: layer PITCH in bytes ([15:0]) */
#define LCDIFV2_CTRLDESCL3_PITCH(x)      (((uint32_t)(x) << 0) & 0x0000FFFFu)
/* CTRLDESCL5: format / blend / enable */
#define LCDIFV2_CTRLDESCL5_BPP(x)        (((uint32_t)(x) << 24) & 0x0F000000u)
#define LCDIFV2_CTRLDESCL5_BPP_RGB565    4u    /* BPP field value for 16bpp RGB565 */
#define LCDIFV2_CTRLDESCL5_BPP_RGB888    8u    /* 24bpp RGB888                     */
#define LCDIFV2_CTRLDESCL5_BPP_ARGB8888  9u    /* 32bpp ARGB8888                   */
#define LCDIFV2_CTRLDESCL5_GLOBAL_ALPHA(x) (((uint32_t)(x) << 16) & 0x00FF0000u)
#define LCDIFV2_CTRLDESCL5_AB_MODE(x)    (((uint32_t)(x) << 0) & 0x00000003u) /* alpha-blend mode */
#define LCDIFV2_CTRLDESCL5_SAFETY_EN     ((uint32_t)(1u<<28))
#define LCDIFV2_CTRLDESCL5_SHADOW_LOAD_EN ((uint32_t)(1u<<30))
#define LCDIFV2_CTRLDESCL5_EN            ((uint32_t)(1u<<31))  /* enable layer DMA */
/* CTRLDESCL6: per-layer background colour */
#define LCDIFV2_CTRLDESCL6_BCLR_B(x)     (((uint32_t)(x) << 0) & 0x000000FFu)
#define LCDIFV2_CTRLDESCL6_BCLR_G(x)     (((uint32_t)(x) << 8) & 0x0000FF00u)
#define LCDIFV2_CTRLDESCL6_BCLR_R(x)     (((uint32_t)(x) << 16) & 0x00FF0000u)
/* CLUT_LOAD */
#define LCDIFV2_CLUT_LOAD_UPDATE_EN      ((uint32_t)(1u<<0))
#define LCDIFV2_CLUT_LOAD_SEL_CLUT_NUM(x)(((uint32_t)(x) << 4) & 0x00000070u)

/* --- MIPI DSI host ---------------------------------------------------------
 * A 4-sub-block design.  Sub-block bases are absolute instance addresses from
 * MIMXRT1176_cm7_COMMON.h (they happen to be contiguous offsets inside the DSI
 * region at 0x4080C000): core @ +0x000, DPI @ +0x200, APB @ +0x280,
 * D-PHY @ +0x300.  IRQ 59 (MIPI_DSI_IRQn), clock gate LPCG131.  None of the
 * DSI sub-blocks has SET/CLR/TOG register aliases. */

/* DSI_HOST core (config/timeouts) @ 0x4080C000 -- PERI_DSI_HOST.h */
#define DSI_HOST_BASE             0x4080C000u
#define DSI_HOST_REG(off)         (*(volatile uint32_t *)(DSI_HOST_BASE + (off)))
#define DSI_HOST_CFG_NUM_LANES            DSI_HOST_REG(0x00)  /* [1:0] NUM_LANES   */
#define DSI_HOST_CFG_NONCONTINUOUS_CLK    DSI_HOST_REG(0x04)  /* bit0 CLK_MODE     */
#define DSI_HOST_CFG_T_PRE                DSI_HOST_REG(0x08)  /* [7:0] TCLK-PRE    */
#define DSI_HOST_CFG_T_POST               DSI_HOST_REG(0x0C)  /* [7:0] TCLK wait   */
#define DSI_HOST_CFG_TX_GAP               DSI_HOST_REG(0x10)  /* [7:0] TX gap      */
#define DSI_HOST_CFG_AUTOINSERT_EOTP      DSI_HOST_REG(0x14)  /* bit0 auto EoTp    */
#define DSI_HOST_CFG_EXTRA_CMDS_AFTER_EOTP DSI_HOST_REG(0x18) /* [7:0]             */
#define DSI_HOST_CFG_HTX_TO_COUNT         DSI_HOST_REG(0x1C)  /* [23:0] HS-TX to   */
#define DSI_HOST_CFG_LRX_H_TO_COUNT       DSI_HOST_REG(0x20)  /* [23:0] LP-RX to   */
#define DSI_HOST_CFG_BTA_H_TO_COUNT       DSI_HOST_REG(0x24)  /* [23:0] BTA to     */
#define DSI_HOST_CFG_TWAKEUP              DSI_HOST_REG(0x28)  /* [18:0] Twakeup    */
#define DSI_HOST_CFG_STATUS_OUT           DSI_HOST_REG(0x2C)  /* RO status         */
#define DSI_HOST_RX_ERROR_STATUS          DSI_HOST_REG(0x30)  /* RO [10:0] errors  */

/* DSI DPI video interface @ 0x4080C200 -- PERI_DSI_HOST_DPI_INTFC.h */
#define DSI_DPI_BASE              0x4080C200u
#define DSI_DPI_REG(off)          (*(volatile uint32_t *)(DSI_DPI_BASE + (off)))
#define DSI_DPI_PIXEL_PAYLOAD_SIZE        DSI_DPI_REG(0x00)  /* [15:0] pixels/line   */
#define DSI_DPI_PIXEL_FIFO_SEND_LEVEL     DSI_DPI_REG(0x04)  /* [15:0]               */
#define DSI_DPI_INTERFACE_COLOR_CODING    DSI_DPI_REG(0x08)  /* [2:0] RGB_CONFIG     */
#define DSI_DPI_PIXEL_FORMAT              DSI_DPI_REG(0x0C)  /* [1:0]                */
#define DSI_DPI_VSYNC_POLARITY            DSI_DPI_REG(0x10)  /* bit0 (1=active high) */
#define DSI_DPI_HSYNC_POLARITY            DSI_DPI_REG(0x14)  /* bit0 (1=active high) */
#define DSI_DPI_VIDEO_MODE                DSI_DPI_REG(0x18)  /* [1:0] VIDEO_MODE     */
#define DSI_DPI_HFP                       DSI_DPI_REG(0x1C)  /* [15:0] H front porch bytes */
#define DSI_DPI_HBP                       DSI_DPI_REG(0x20)  /* [15:0] H back porch bytes  */
#define DSI_DPI_HSA                       DSI_DPI_REG(0x24)  /* [15:0] H sync-active bytes */
#define DSI_DPI_ENABLE_MULT_PKTS          DSI_DPI_REG(0x28)  /* bit0                 */
#define DSI_DPI_VBP                       DSI_DPI_REG(0x2C)  /* [7:0] V back porch lines   */
#define DSI_DPI_VFP                       DSI_DPI_REG(0x30)  /* [7:0] V front porch lines  */
#define DSI_DPI_BLLP_MODE                 DSI_DPI_REG(0x34)  /* bit0 LP during BLLP  */
#define DSI_DPI_USE_NULL_PKT_BLLP         DSI_DPI_REG(0x38)  /* bit0                 */
#define DSI_DPI_VACTIVE                   DSI_DPI_REG(0x3C)  /* [13:0] active lines  */
/* DPI field masks */
#define DSI_DPI_INTERFACE_COLOR_CODING_MASK ((uint32_t)0x7u)
#define DSI_DPI_PIXEL_FORMAT_MASK           ((uint32_t)0x3u)
#define DSI_DPI_VIDEO_MODE_MASK             ((uint32_t)0x3u)
#define DSI_DPI_VACTIVE_MASK                ((uint32_t)0x3FFFu)

/* DSI APB packet interface (DCS/generic writes) @ 0x4080C280
 * -- PERI_DSI_HOST_APB_PKT_IF.h */
#define DSI_APB_BASE              0x4080C280u
#define DSI_APB_REG(off)          (*(volatile uint32_t *)(DSI_APB_BASE + (off)))
#define DSI_APB_TX_PAYLOAD                DSI_APB_REG(0x00)  /* payload FIFO write */
#define DSI_APB_PKT_CONTROL               DSI_APB_REG(0x04)  /* [26:0] header+flags*/
#define DSI_APB_SEND_PACKET               DSI_APB_REG(0x08)  /* bit0 TX_SEND       */
#define DSI_APB_PKT_STATUS                DSI_APB_REG(0x0C)  /* RO [8:0]           */
#define DSI_APB_PKT_FIFO_WR_LEVEL         DSI_APB_REG(0x10)  /* RO [15:0]          */
#define DSI_APB_PKT_FIFO_RD_LEVEL         DSI_APB_REG(0x14)  /* RO [15:0]          */
#define DSI_APB_PKT_RX_PAYLOAD            DSI_APB_REG(0x18)  /* RO                 */
#define DSI_APB_PKT_RX_PKT_HEADER         DSI_APB_REG(0x1C)  /* RO [23:0]          */
#define DSI_APB_IRQ_STATUS                DSI_APB_REG(0x20)  /* RO                 */
#define DSI_APB_IRQ_STATUS2               DSI_APB_REG(0x24)  /* RO [2:0]           */
#define DSI_APB_IRQ_MASK                  DSI_APB_REG(0x28)
#define DSI_APB_IRQ_MASK2                 DSI_APB_REG(0x2C)  /* [2:0]              */
/* PKT_CONTROL packing (word count / virtual channel / data type / flags).
 * These sub-field positions are not broken out in PERI_DSI_HOST_APB_PKT_IF.h
 * (which gives only the aggregate [26:0] CTRL field); the layout is
 * transcribed from the NXP driver fsl_mipi_dsi.c (BSD-3-Clause). */
#define DSI_APB_PKT_CONTROL_WORD_COUNT(wc) ((((uint32_t)(wc)) << 0) & 0x0000FFFFu)   /* [15:0] */
#define DSI_APB_PKT_CONTROL_VC(vc)         ((((uint32_t)(vc)) << 16) & 0x00030000u)  /* [17:16] virtual channel */
#define DSI_APB_PKT_CONTROL_HEADER_TYPE(t) ((((uint32_t)(t)) << 18) & 0x00FC0000u)   /* [23:18] DSI data type */
#define DSI_APB_PKT_CONTROL_HS             ((uint32_t)(1u<<24))      /* send in high-speed      */
#define DSI_APB_PKT_CONTROL_BTA            ((uint32_t)(1u<<25))      /* bus-turnaround after tx */
#define DSI_APB_PKT_CONTROL_BTA_ONLY       ((uint32_t)(1u<<26))
#define DSI_APB_SEND_PACKET_TX_SEND        ((uint32_t)(1u<<0))
/* PKT_STATUS bits (from fsl_mipi_dsi.h kDSI_Apb* enum, BSD-3-Clause) */
#define DSI_APB_PKT_STATUS_NOT_IDLE            ((uint32_t)(1u<<0))
#define DSI_APB_PKT_STATUS_TX_DONE             ((uint32_t)(1u<<1))
#define DSI_APB_PKT_STATUS_RX_CONTROL          ((uint32_t)(1u<<2))
#define DSI_APB_PKT_STATUS_TX_OVERFLOW         ((uint32_t)(1u<<3))
#define DSI_APB_PKT_STATUS_TX_UNDERFLOW        ((uint32_t)(1u<<4))
#define DSI_APB_PKT_STATUS_RX_OVERFLOW         ((uint32_t)(1u<<5))
#define DSI_APB_PKT_STATUS_RX_UNDERFLOW        ((uint32_t)(1u<<6))
#define DSI_APB_PKT_STATUS_RX_HEADER_RECEIVED  ((uint32_t)(1u<<7))
#define DSI_APB_PKT_STATUS_RX_PACKET_RECEIVED  ((uint32_t)(1u<<8))

/* DSI D-PHY (NXP FDSOI28) @ 0x4080C300 -- PERI_DSI_HOST_NXP_FDSOI28_DPHY_INTFC.h
 * PLL VCO = (refClk / CN) * CM; CO is the output post-divider. */
#define DSI_DPHY_BASE             0x4080C300u
#define DSI_DPHY_REG(off)         (*(volatile uint32_t *)(DSI_DPHY_BASE + (off)))
#define DSI_DPHY_PD_TX                    DSI_DPHY_REG(0x00)  /* bit0 power-down TX */
#define DSI_DPHY_M_PRG_HS_PREPARE         DSI_DPHY_REG(0x04)  /* [1:0]  data THS-prepare  */
#define DSI_DPHY_MC_PRG_HS_PREPARE        DSI_DPHY_REG(0x08)  /* bit0   clock THS-prepare */
#define DSI_DPHY_M_PRG_HS_ZERO            DSI_DPHY_REG(0x0C)  /* [4:0]  data THS-zero     */
#define DSI_DPHY_MC_PRG_HS_ZERO           DSI_DPHY_REG(0x10)  /* [5:0]  clock THS-zero    */
#define DSI_DPHY_M_PRG_HS_TRAIL           DSI_DPHY_REG(0x14)  /* [3:0]  data THS-trail    */
#define DSI_DPHY_MC_PRG_HS_TRAIL          DSI_DPHY_REG(0x18)  /* [3:0]  clock THS-trail   */
#define DSI_DPHY_PD_PLL                   DSI_DPHY_REG(0x1C)  /* bit0 power-down PLL */
#define DSI_DPHY_TST                      DSI_DPHY_REG(0x20)  /* [5:0] test         */
#define DSI_DPHY_CN                       DSI_DPHY_REG(0x24)  /* [4:0] PLL N divider*/
#define DSI_DPHY_CM                       DSI_DPHY_REG(0x28)  /* [7:0] PLL M divider*/
#define DSI_DPHY_CO                       DSI_DPHY_REG(0x2C)  /* [1:0] PLL out div  */
#define DSI_DPHY_LOCK                     DSI_DPHY_REG(0x30)  /* RO bit0 PLL locked */
#define DSI_DPHY_LOCK_BYP                 DSI_DPHY_REG(0x34)  /* bit0               */
#define DSI_DPHY_TX_RCAL                  DSI_DPHY_REG(0x38)  /* [1:0]              */
#define DSI_DPHY_AUTO_PD_EN               DSI_DPHY_REG(0x3C)  /* bit0               */
#define DSI_DPHY_RXLPRP                   DSI_DPHY_REG(0x40)  /* [1:0]              */
#define DSI_DPHY_RXCDRP                   DSI_DPHY_REG(0x44)  /* [1:0]              */
/* D-PHY field masks */
#define DSI_DPHY_CN_MASK          ((uint32_t)0x1Fu)
#define DSI_DPHY_CM_MASK          ((uint32_t)0xFFu)
#define DSI_DPHY_CO_MASK          ((uint32_t)0x3u)
#define DSI_DPHY_LOCK_MASK        ((uint32_t)0x1u)

/* --- Display-interface plumbing (SoC-level, OUTSIDE the DSI register file) --
 * The MIPI-DSI host does NOT come out of chip reset usable, and nothing in its
 * own register file says so.  Three separate blocks gate it, and NXP's
 * BOARD_InitDisplayInterface() (mcuxsdk-examples _boards/evkbmimxrt1170/
 * display_support.c, BSD-3-Clause) touches all three:
 *   - IOMUXC_GPR62[19:16]: the four MIPI-DSI clock-domain soft resets.  ALL
 *     ACTIVE LOW and NOT self-clearing -- writing 1 RELEASES that domain, and
 *     the POR value 0000_02DBh has all four CLEAR ("by default it holds the
 *     MIPI DSI in reset state").  GPR62 must be READ-MODIFY-WRITTEN: bits 13:0
 *     of that POR value are D-PHY trim defaults that a blind write destroys.
 *   - VIDEO_MUX VID_MUX_CTRL[MIPI_DSI_SEL]: which display controller feeds the
 *     DSI's DPI input (POR 0 = eLCDIF, NOT LCDIFv2).
 *   - PGMC_BPC4 BPC_POWER_CTRL: the MIPI power domain's soft power-switch /
 *     isolation controls (POR 0 = powered off, isolated).
 * Addresses/offsets/bit positions from the RM sections cited per line below,
 * cross-checked against the SDK peripheral-access headers PERI_IOMUXC_GPR.h /
 * PERI_VIDEO_MUX.h / PERI_PGMC.h (BSD-3-Clause; facts only, re-expressed in
 * this file's #define idiom -- NXP struct typedefs NOT copied). */

/* IOMUXC_GPR62 @ IOMUXC_GPR + F8h -- RM 12.4.5.58 (POR 0000_02DBh, RW). */
#define IOMUXC_GPR62              (*(volatile uint32_t *)(IOMUXC_GPR_BASE + 0xF8u))
/* GPR62 MIPI-DSI soft resets -- RM 12.4.5.58.4.  Active LOW: set = out of
 * reset.  RM field names are by clock domain ("APB"/"Byte"/"Pixel"/"Escape");
 * the mnemonics below are PERI_IOMUXC_GPR.h's. */
#define IOMUXC_GPR62_MIPI_DSI_PCLK_SOFT_RESET_N  ((uint32_t)(1u<<16))  /* APB clock domain    */
#define IOMUXC_GPR62_MIPI_DSI_BYTE_SOFT_RESET_N  ((uint32_t)(1u<<17))  /* byte clock domain   */
#define IOMUXC_GPR62_MIPI_DSI_DPI_SOFT_RESET_N   ((uint32_t)(1u<<18))  /* pixel clock domain  */
#define IOMUXC_GPR62_MIPI_DSI_ESC_SOFT_RESET_N   ((uint32_t)(1u<<19))  /* escape clock domain */

/* VIDEO_MUX @ 4081_8000h -- RM 50.6.1.1.  Every control register is a 16-byte
 * RW/SET/CLR/TOG quad; all reset to 0000_0000h.  v1 only needs VID_MUX_CTRL. */
#define VIDEO_MUX_BASE            0x40818000u
#define VIDEO_MUX_REG(off)        (*(volatile uint32_t *)(VIDEO_MUX_BASE + (off)))
#define VIDEO_MUX_VID_MUX_CTRL        VIDEO_MUX_REG(0x00)  /* RM 50.6.1.2            */
#define VIDEO_MUX_VID_MUX_CTRL_SET    VIDEO_MUX_REG(0x04)  /* write 1 = set bit      */
#define VIDEO_MUX_VID_MUX_CTRL_CLR    VIDEO_MUX_REG(0x08)  /* write 1 = clear bit    */
#define VIDEO_MUX_VID_MUX_CTRL_TOG    VIDEO_MUX_REG(0x0C)  /* write 1 = toggle bit   */
/* VID_MUX_CTRL fields -- RM 50.6.1.2. */
#define VIDEO_MUX_VID_MUX_CTRL_CSI_SEL      ((uint32_t)(1u<<0))  /* CSI sensor data source     */
#define VIDEO_MUX_VID_MUX_CTRL_LCDIF2_SEL   ((uint32_t)(1u<<1))  /* LCDIFv2 sensor data source */
#define VIDEO_MUX_VID_MUX_CTRL_MIPI_DSI_SEL ((uint32_t)(1u<<2))  /* 0=DSI from eLCDIF, 1=from LCDIFv2 */
#define VIDEO_MUX_VID_MUX_CTRL_PARA_LCD_SEL ((uint32_t)(1u<<3))  /* parallel LCDIF data source */
/* VIDEO_MUX clock gate: LPCG136 (kCLOCK_Video_Mux, fsl_clock.h) =
 * 0x40CC6000 + 136*0x20.  BOARD_InitDisplayInterface() ungates it first. */
#define CCM_LPCG136_DIRECT        (*(volatile uint32_t *)0x40CC7100u)

/* PGMC basic power controllers: PGMC_BPCn @ 40C8_8000h + n*200h -- RM 20.6.1.1.
 * BPC4 (40C8_8800h) is the MIPI power domain. */
#define PGMC_BPC4_BASE            0x40C88800u
#define PGMC_BPC4_BPC_POWER_CTRL  (*(volatile uint32_t *)(PGMC_BPC4_BASE + 0x14u)) /* RM 20.6.1.4, POR 0 */
/* BPC_POWER_CTRL fields -- RM 20.6.1.4.4. */
#define PGMC_BPC_POWER_CTRL_PSW_ON_SOFT   ((uint32_t)(1u<<10))  /* software power-on trigger    */
#define PGMC_BPC_POWER_CTRL_ISO_OFF_SOFT  ((uint32_t)(1u<<11))  /* software isolation-off trigger */

/* --- Display clocks (Task 6 consumes these) --------------------------------
 * VIDEO PLL in ANATOP (base 0x40C84000, same block as ARM/AUDIO PLL above);
 * offsets from PERI_ANADIG_PLL.h.  On RT1176 the fractional-N config
 * (DIV_SELECT/NUM/DENOM) is normally written via the ANATOP AI protocol (cf.
 * the AUDIO PLL AI regs above) unless PLL_VIDEO_CTRL.CONTROL_MODE selects
 * direct mode -- Task 6 decides.  CCM CLOCK_ROOT/LPCG addresses follow the
 * existing conventions: CLOCK_ROOTn @ 0x40CC0000 + n*0x80, LPCGn @ 0x40CC6000
 * + n*0x20.  Root/gate indices from fsl_clock.h. */
#define ANADIG_PLL_VIDEO_CTRL         (*(volatile uint32_t *)0x40C84350u) /* ANATOP +0x350 */
#define ANADIG_PLL_VIDEO_SS           (*(volatile uint32_t *)0x40C84360u) /* +0x360 spread-spectrum */
#define ANADIG_PLL_VIDEO_DENOMINATOR  (*(volatile uint32_t *)0x40C84370u) /* +0x370 [29:0] */
#define ANADIG_PLL_VIDEO_NUMERATOR    (*(volatile uint32_t *)0x40C84380u) /* +0x380 [29:0] */
#define ANADIG_PLL_VIDEO_DIV_SELECT   (*(volatile uint32_t *)0x40C84390u) /* +0x390 [6:0]  */
#define ANADIG_PLL_VIDEO_CTRL_ENABLE_CLK   (1u<<13)
#define ANADIG_PLL_VIDEO_CTRL_GATE         (1u<<14)   /* PLL_VIDEO_GATE          */
#define ANADIG_PLL_VIDEO_CTRL_COUNTER_CLR  (1u<<24)
#define ANADIG_PLL_VIDEO_CTRL_STABLE       (1u<<29)   /* RO PLL_VIDEO_STABLE     */
#define ANADIG_PLL_VIDEO_CTRL_AI_BUSY      (1u<<30)   /* RO                      */
#define ANADIG_PLL_VIDEO_CTRL_CONTROL_MODE (1u<<31)   /* 1=direct-register mode  */
#define ANADIG_PLL_VIDEO_DIV_SELECT_MASK   ((uint32_t)0x7Fu)
#define ANADIG_PLL_VIDEO_NUM_MASK          ((uint32_t)0x3FFFFFFFu)
#define ANADIG_PLL_VIDEO_DENOM_MASK        ((uint32_t)0x3FFFFFFFu)
#define ANADIG_PLL_VIDEO_SS_STEP(x)        (((uint32_t)(x) << 0) & 0x00007FFFu)
#define ANADIG_PLL_VIDEO_SS_ENABLE         (1u<<15)
#define ANADIG_PLL_VIDEO_SS_STOP(x)        (((uint32_t)(x) << 16) & 0xFFFF0000u)
/* LCDIFv2 clock: CLOCK_ROOT70 (kCLOCK_Root_Lcdifv2) + gate LPCG130 */
#define CCM_CLOCK_ROOT70_CONTROL      (*(volatile uint32_t *)0x40CC2300u)  /* LCDIFv2 root */
#define CCM_LPCG130_DIRECT            (*(volatile uint32_t *)0x40CC7040u)  /* LCDIFv2 gate */
/* MIPI clocks: CLOCK_ROOT71 = MIPI_REF (kCLOCK_Root_Mipi_Ref), CLOCK_ROOT72 =
 * MIPI_ESC (kCLOCK_Root_Mipi_Esc); DSI-host gate LPCG131 (kCLOCK_Mipi_Dsi).
 * On both roots the VIDEO PLL output is mux select 7. */
#define CCM_CLOCK_ROOT71_CONTROL      (*(volatile uint32_t *)0x40CC2380u)  /* MIPI_REF root */
#define CCM_CLOCK_ROOT72_CONTROL      (*(volatile uint32_t *)0x40CC2400u)  /* MIPI_ESC root */
#define CCM_LPCG131_DIRECT            (*(volatile uint32_t *)0x40CC7060u)  /* MIPI_DSI gate */
#define CCM_CLOCK_ROOT_MUX_VIDEO_PLL  7u   /* MUX value: VideoPllOut on LCDIFv2/MIPI_REF/MIPI_ESC roots */
/* MUX value 1 = MuxOsc24MOut (the 24 MHz crystal oscillator) on those same
 * three roots -- fsl_clock.h kCLOCK_{LCDIFV2,MIPI_REF,MIPI_ESC}_ClockRoot_
 * MuxOsc24MOut, all = 1U.  BOARD_InitMipiDsiClock() uses it (mux 1, div 1) for
 * the MIPI D-PHY reference clock. */
#define CCM_CLOCK_ROOT_MUX_OSC_24M    1u

#endif
