/* enet.c -- NXP i.MX RT1176 ENET (10/100 Freescale FEC) MAC bring-up and raw
 * flat-buffer TX/RX on DMAMEM descriptor rings.
 *
 * Ported from the FNET FEC driver (src/port/netif/fec/fnet_fec.c) -- same IP --
 * and the NXP SDK evkbmimxrt1170 enet/txrx_transfer board bring-up
 * (hardware_init.c + pin_mux.c).  Register/bit defs live in imxrt1176.h.
 *
 * ============================ ENDIANNESS (★) ============================
 * The legacy 8-byte buffer descriptor { status, length, buf } is defined
 * big-endian.  Two self-consistent ways to drive it against the RT1176 FEC and
 * the QEMU imx.enet model (hw/net/imx_fec.c) exist:
 *   (A) ECR.DBSWP set   -> controller reads native little-endian BD words
 *                          (SDK convention: struct {length, control, buffer}).
 *   (B) ECR.DBSWP clear -> BD words are big-endian in memory; software swaps
 *                          them with htons()/htonl() (the FNET convention).
 * This driver uses (B): a faithful FNET port.  The BD status/length halfwords
 * are byte-swapped with bswap16() and the buffer pointer with bswap32() before
 * the controller reads them, and ECR is left with DBSWP CLEAR.  QEMU's model
 * confirms this pairing: imx_fec_read_bd()/write_bd() byteswap the descriptor
 * *iff* !(ECR & DSBWP), and its own comment names the FNET stack as the
 * DBSWP-clear user.  (The plan's headline suggested pairing DBSWP-set WITH a
 * bswap16 helper; those two are mutually exclusive -- setting DBSWP AND swapping
 * would double-swap and the frames would not move.  Verified against the QEMU
 * model + FNET; see the task report.)
 */
#include "imxrt1176.h"
#include "core_pins.h"   /* delayMicroseconds() */
#include "enet.h"
#include <string.h>

/* ---- descriptor ring / buffer sizing ---- */
#define ENET_RXBD_NUM 4
#define ENET_TXBD_NUM 4
#define ENET_BUF_SZ   1536

/* Legacy 8-byte buffer descriptor (FNET fnet_fec_buf_desc_t layout: status @0,
 * length @2, buffer pointer @4).  status/length are stored byte-swapped; see
 * the endianness note above. */
typedef struct __attribute__((packed)) {
    volatile uint16_t status;   /* volatile: the MAC mutates these under us --  */
    volatile uint16_t length;   /* without it -Os hoists the busy-wait load out */
    volatile uint32_t buf;      /* of enet_send_frame's spin (regression vs FNET) */
} enet_bd_t;

/* ERR050396: ENET bus-master writes to CM7 TCM can corrupt via the NIC301
 * sparse-write path -- DMA memory MUST live in OCRAM (or external RAM), never
 * DTCM/ITCM.  DMAMEM (.dmabuffers) is linked into OCRAM (imxrt1176.ld). */
DMAMEM __attribute__((aligned(64))) static enet_bd_t rx_bd[ENET_RXBD_NUM];
DMAMEM __attribute__((aligned(64))) static enet_bd_t tx_bd[ENET_TXBD_NUM];
DMAMEM __attribute__((aligned(64))) static uint8_t   rx_buf[ENET_RXBD_NUM][ENET_BUF_SZ];
DMAMEM __attribute__((aligned(64))) static uint8_t   tx_buf[ENET_TXBD_NUM][ENET_BUF_SZ];

static uint8_t tx_idx;   /* next TX descriptor to fill */
static uint8_t rx_idx;   /* next RX descriptor to inspect */

/* ---- byte-swap + barrier helpers ---- */
static inline uint16_t bswap16(uint16_t v)
{
    return (uint16_t)((v >> 8) | (v << 8));
}
static inline uint32_t bswap32(uint32_t v)
{
    return ((v & 0x000000FFu) << 24) | ((v & 0x0000FF00u) << 8) |
           ((v & 0x00FF0000u) >> 8)  | ((v & 0xFF000000u) >> 24);
}
static inline void enet_dmb(void)
{
    __asm__ volatile ("dmb" ::: "memory");
}

/* ENET_*_SELECT_INPUT daisy (input-mux) registers.  These are one-off IOMUXC
 * input-select regs, not emitted into imxrt1176.h; addresses extracted and
 * VERIFIED from fsl_iomuxc.h IOMUXC_GPIO_*_ENET_* macros. */
#define ENET_MDIO_SELECT_INPUT       (*(volatile uint32_t *)0x400E84ACu)
#define ENET_REF_CLK_SELECT_INPUT    (*(volatile uint32_t *)0x400E84A8u) /* REF_CLK (DISP_B2_05) */
#define ENET_RXDATA0_SELECT_INPUT    (*(volatile uint32_t *)0x400E84B0u)
#define ENET_RXDATA1_SELECT_INPUT    (*(volatile uint32_t *)0x400E84B4u)
#define ENET_RXEN_SELECT_INPUT       (*(volatile uint32_t *)0x400E84B8u)
#define ENET_RXERR_SELECT_INPUT      (*(volatile uint32_t *)0x400E84BCu)

#define IOMUXC_SION 0x10u   /* Software Input On Field, bit 4 of SW_MUX_CTL */

/* MDIO/MDC source = Bus root clock (SDK EXAMPLE_CLOCK_FREQ =
 * CLOCK_GetRootClockFreq(kCLOCK_Root_Bus); RT1176 BOARD_BootClockRUN Bus root =
 * 240 MHz).  MDC must be <= 2.5 MHz: MII_SPEED = ceil(src/(2*MDC)) - 1 -> 47.
 * HW/Task-4 only -- the QEMU gate masks MSCR to 0xfe and never toggles MDC. */
#define ENET_MDIO_SRC_HZ 240000000u
#define ENET_MDC_HZ      2500000u
#define ENET_MII_SPEED_N (((ENET_MDIO_SRC_HZ + (2u * ENET_MDC_HZ) - 1u) / (2u * ENET_MDC_HZ)) - 1u)

/* --------------------------------------------------------------------------
 * Clock init (Sub-step 3f).  QEMU no-op (no CCM model); HW-critical, and the
 * one part the gate cannot verify -- flagged HW-unverified in the report.
 * -------------------------------------------------------------------------- */
static void enet_clock_init(void)
{
    /* Route the ENET1 clock root (root 51) to SysPll1Div2 (mux 4), divide-by-10
     * -> 50 MHz.  Matches the SDK enet/txrx_transfer BOARD_InitModuleClock
     * clock_root_config_t {.mux = 4, .div = 10}.  The CCM ROOT DIV field encodes
     * (divider - 1), so .div=10 -> field 9. */
    CCM_CLOCK_ROOT51_CONTROL = CCM_CLOCK_ROOT_CONTROL_MUX(4) |
                               CCM_CLOCK_ROOT_CONTROL_DIV(9);
    /* Ungate the ENET peripheral clock (LPCG112 / kCLOCK_Enet). */
    CCM_LPCG112_DIRECT = 1u;

    /* NOTE: the SDK ALSO calls CLOCK_InitSysPll1({.pllDiv2En = true}) to power
     * SysPll1 and its /2 output before this.  We deliberately do NOT fabricate a
     * PLL bring-up here: re-initing a boot-ROM-managed PLL can reset-loop this
     * core, and SysPll1's boot state is board-dependent.  SysPll1Div2 enablement
     * is a HW follow-up (board/Task-4).  On QEMU none of this matters. */
}

/* --------------------------------------------------------------------------
 * RMII + MDIO pin mux (Sub-step 3d).  QEMU no-op (no IOMUXC model); HW-critical.
 * ALT/SION/daisy/pad-ctl VERIFIED against enet/txrx_transfer pin_mux.c.
 * -------------------------------------------------------------------------- */
static void enet_pins_init(void)
{
    /* MDC / MDIO: ALT3, no SION; SDK leaves PAD_CTL at default (no write). */
    IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_32 = 3u;                 /* ENET_MDC  */
    IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_33 = 3u;                 /* ENET_MDIO */
    ENET_MDIO_SELECT_INPUT = 1u;

    /* TXD0/TXD1/TX_EN: ALT1, no SION, PAD_CTL = 0x02. */
    IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_02 = 1u;
    IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_02 = 0x02u;
    IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_03 = 1u;
    IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_03 = 0x02u;
    IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_04 = 1u;
    IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_04 = 0x02u;

    /* REF_CLK: ALT2 + SION, daisy=1, PAD_CTL = 0x03 (50 MHz RMII ref clock). */
    IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_05 = 2u | IOMUXC_SION;
    ENET_REF_CLK_SELECT_INPUT = 1u;
    IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_05 = 0x03u;

    /* RXD0/RXD1: ALT1 + SION, daisy=1, PAD_CTL = 0x06. */
    IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_06 = 1u | IOMUXC_SION;
    ENET_RXDATA0_SELECT_INPUT = 1u;
    IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_06 = 0x06u;
    IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_07 = 1u | IOMUXC_SION;
    ENET_RXDATA1_SELECT_INPUT = 1u;
    IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_07 = 0x06u;

    /* RX_EN/RX_ER: ALT1, no SION, daisy=1, PAD_CTL = 0x06. */
    IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_08 = 1u;
    ENET_RXEN_SELECT_INPUT = 1u;
    IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_08 = 0x06u;
    IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_09 = 1u;
    ENET_RXERR_SELECT_INPUT = 1u;
    IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_09 = 0x06u;

    /* 50 MHz ENET_REF_CLK is an OUTPUT from the SoC to the external PHY. */
    IOMUXC_GPR_GPR4 |= IOMUXC_GPR_GPR4_ENET_REF_CLK_DIR;
    /* ERR050396: with the DMA buffers in OCRAM, clear CACHE_ENET so ENET writes
     * bypass the CM7-TCM sparse-write path. */
    IOMUXC_GPR_GPR28 &= ~IOMUXC_GPR_GPR28_CACHE_ENET;
}

/* --------------------------------------------------------------------------
 * PHY hardware reset (Sub-step 3e).  QEMU no-op; HW-critical.  RTL8201 on the
 * EVKB is reset via GPIO12_IO12 (pad GPIO_LPSR_12).  Per SDK hardware_init.c.
 * -------------------------------------------------------------------------- */
static void enet_phy_reset(void)
{
    IOMUXC_SW_MUX_CTL_PAD_GPIO_LPSR_12 = 0xAu;   /* GPIO12_IO12 (ALT 0xA) */
    IOMUXC_SW_PAD_CTL_PAD_GPIO_LPSR_12 = 0x0Eu;  /* PAD_CTL per SDK pin_mux.c */

    GPIO12_GDIR |= (1u << 12);                   /* output */
    GPIO12_DR_CLEAR = (1u << 12);                /* assert reset (low) */
    delayMicroseconds(10000);                    /* >= 10 ms low */
    GPIO12_DR_SET = (1u << 12);                  /* release reset (high) */
    delayMicroseconds(150000);                   /* >= 150 ms for PHY ready */
}

/* --------------------------------------------------------------------------
 * Descriptor ring init (Sub-step 3c).  Ported from fnet_fec.c ring setup.
 * status/length/buf are stored byte-swapped (DBSWP clear -- see note).
 * -------------------------------------------------------------------------- */
static void enet_ring_init(void)
{
    int i;
    for (i = 0; i < ENET_TXBD_NUM; i++) {
        tx_bd[i].status = bswap16(ENET_TXBD_L | ENET_TXBD_TC);
        tx_bd[i].length = 0u;
        tx_bd[i].buf    = bswap32((uint32_t)(uintptr_t)&tx_buf[i][0]);
    }
    for (i = 0; i < ENET_RXBD_NUM; i++) {
        rx_bd[i].status = bswap16(ENET_RXBD_E);   /* empty: owned by the MAC */
        rx_bd[i].length = 0u;
        rx_bd[i].buf    = bswap32((uint32_t)(uintptr_t)&rx_buf[i][0]);
    }
    /* Wrap bit on the last descriptor of each ring. */
    tx_bd[ENET_TXBD_NUM - 1].status |= bswap16(ENET_TXBD_W);
    rx_bd[ENET_RXBD_NUM - 1].status |= bswap16(ENET_RXBD_W);

    tx_idx = 0u;
    rx_idx = 0u;
}

/* --------------------------------------------------------------------------
 * enet_init
 * -------------------------------------------------------------------------- */
void enet_init(const uint8_t mac[6])
{
    enet_clock_init();

    /* Reset the MAC and wait for the reset to self-clear.  Bounded by a loop
     * cap so a dead ENET clock on real HW can't hang here forever (QEMU clears
     * RESET immediately); on timeout we press on best-effort (init is void). */
    ENET_ECR = ENET_ECR_RESET;
    {
        uint32_t timeout = 1000000u;
        while ((ENET_ECR & ENET_ECR_RESET) && (--timeout != 0u)) {
            /* spin */
        }
    }

    enet_pins_init();
    enet_phy_reset();
    enet_ring_init();

    /* Mask + clear all MAC interrupts (this driver polls). */
    ENET_EIMR = 0u;
    ENET_EIR  = 0xFFFFFFFFu;

    /* Station address: PALR = MAC[0..3], PAUR[31:16] = MAC[4..5]. */
    ENET_PALR = ((uint32_t)mac[0] << 24) | ((uint32_t)mac[1] << 16) |
                ((uint32_t)mac[2] << 8)  | ((uint32_t)mac[3]);
    ENET_PAUR = ((uint32_t)mac[4] << 24) | ((uint32_t)mac[5] << 16);

    /* MDIO clock (MDC <= 2.5 MHz). HW/Task-4 only; gate ignores MSCR. */
    ENET_MSCR = ENET_MSCR_MII_SPEED(ENET_MII_SPEED_N);

    /* Hash tables (GAUR/GALR/IAUR/IALR) are already 0 after the ECR reset. */

    /* Receiver: RMII mode, full duplex (no DRT), max frame length 1518. */
    ENET_RCR = ENET_RCR_MII_MODE | ENET_RCR_RMII_MODE | ENET_RCR_MAX_FL(1518u);
    /* Transmitter: full duplex. */
    ENET_TCR = ENET_TCR_FDEN;

    /* Descriptor ring base pointers + max receive buffer size. */
    ENET_RDSR = (uint32_t)(uintptr_t)&rx_bd[0];
    ENET_TDSR = (uint32_t)(uintptr_t)&tx_bd[0];
    ENET_MRBR = ENET_BUF_SZ;

    /* Enable the MAC.  DBSWP is left CLEAR (software byte-swaps the BDs -- see
     * the endianness note at the top of this file). */
    ENET_ECR = ENET_ECR_ETHEREN;

    /* Arm the receive ring. */
    ENET_RDAR = ENET_RDAR_ACTIVE;
}

/* --------------------------------------------------------------------------
 * enet_send_frame  -- returns 0 on success, -2 on TX-descriptor timeout.
 * Ported from fnet_fec_output().
 * -------------------------------------------------------------------------- */
int enet_send_frame(const uint8_t *frame, uint16_t len)
{
    enet_bd_t *bd = &tx_bd[tx_idx];
    uint32_t timeout = 2000000u;      /* loop-iteration bound, NOT wall-clock */
    uint16_t st;

    if (len > ENET_BUF_SZ) {
        return -1;                    /* frame too large for one TX buffer */
    }

    /* Wait (bounded) for this descriptor to be free (R = ready/owned clears when
     * the MAC has transmitted it).  NOTE: v1 does not inspect the written-back
     * TX completion error bits (UN underrun / RL retry-limit / LC late-coll). */
    while (bswap16(bd->status) & ENET_TXBD_R) {
        if (--timeout == 0u) {
            return -2;
        }
    }

    memcpy(&tx_buf[tx_idx][0], frame, len);
    /* Pad runt frames to the 60-byte Ethernet minimum; the MAC appends the
     * 4-byte FCS for 64 on the wire. */
    if (len < 60u) {
        memset(&tx_buf[tx_idx][len], 0, 60u - len);
        len = 60u;
    }

    bd->length = bswap16(len);
    st = ENET_TXBD_R | ENET_TXBD_L | ENET_TXBD_TC;
    if (tx_idx == (ENET_TXBD_NUM - 1)) {
        st |= ENET_TXBD_W;
    }
    bd->status = bswap16(st);

    enet_dmb();                       /* complete BD writes before DMA start */
    ENET_TDAR = ENET_TDAR_ACTIVE;     /* kick the transmit ring */

    tx_idx = (uint8_t)((tx_idx + 1u) % ENET_TXBD_NUM);
    return 0;
}

/* --------------------------------------------------------------------------
 * enet_read_frame -- returns 1 if a frame was copied out, 0 if none pending,
 * -1 if the pending frame had a MAC-layer error (dropped).
 * Ported from _fnet_fec_input() / _fnet_fec_rx_buf_next().
 * -------------------------------------------------------------------------- */
int enet_read_frame(uint8_t *buf, uint16_t *len)
{
    enet_bd_t *bd = &rx_bd[rx_idx];
    uint16_t status = bswap16(bd->status);
    uint16_t st;
    int ret;

    *len = 0;                         /* always define the out-param */

    if (status & ENET_RXBD_E) {
        return 0;                     /* still empty/owned by the MAC engine */
    }

    /* Deliver a frame ONLY if it is a complete, error-free, single-BD frame:
     * L (last) set AND no error bits.  This v1 driver has no reassembly, and
     * an over-length frame the FEC splits across BDs carries L/error bits only
     * on the FINAL fragment -- a lead fragment comes back with no error and
     * length == MRBR (ENET_BUF_SZ), which would memcpy past the caller's buffer.
     * Normal <=1518B frames always fit one BD with L set, so this rejects
     * nothing legitimate; an over-length single frame already trips LG->ERR. */
    if (((status & ENET_RXBD_L) == 0u) || (status & ENET_RXBD_ERR)) {
        ret = -1;                     /* fragment or MAC-layer error -> drop */
    } else {
        uint16_t n = bswap16(bd->length);
        if (n > ENET_BUF_SZ) {        /* defensive: single-BD length <= MRBR */
            n = ENET_BUF_SZ;
        }
        memcpy(buf, &rx_buf[rx_idx][0], n);
        *len = n;
        ret = 1;
    }

    /* Re-arm this descriptor: keep only the W bit, set E (hand back to MAC). */
    st = ENET_RXBD_E;
    if (rx_idx == (ENET_RXBD_NUM - 1)) {
        st |= ENET_RXBD_W;
    }
    bd->status = bswap16(st);

    enet_dmb();
    ENET_RDAR = ENET_RDAR_ACTIVE;     /* signal an empty buffer is available */

    rx_idx = (uint8_t)((rx_idx + 1u) % ENET_RXBD_NUM);
    return ret;
}

/* --------------------------------------------------------------------------
 * MDIO / PHY link.  Ported from fnet_fec.c _fnet_fec_phy_read/_fnet_fec_phy_write
 * (~:925/:984); field defs (ENET_MMFR_*, ENET_EIR_MII) live in imxrt1176.h and
 * are verified bit-for-bit against FNET's FNET_FEC_MMFR_xxx / FNET_FEC_EIR_MII.
 * This driver polls (EIMR masked to 0 in enet_init), so unlike FNET's
 * mask/restore-EIMR dance around each MII transfer, no EIMR save/restore is
 * needed here.  (PHY on the EVKB is an RTL8201 at MDIO address 0x03.)
 * -------------------------------------------------------------------------- */
uint16_t enet_mdio_read(uint8_t phy, uint8_t reg) {
    ENET_EIR = ENET_EIR_MII;                 /* clear MII-done */
    ENET_MMFR = ENET_MMFR_ST_01 | ENET_MMFR_OP_READ | ENET_MMFR_PA(phy)
              | ENET_MMFR_RA(reg) | ENET_MMFR_TA_10;
    uint32_t spin = 0;
    while (!(ENET_EIR & ENET_EIR_MII)) { if (++spin > 1000000u) return 0xffffu; }
    ENET_EIR = ENET_EIR_MII;
    return (uint16_t)(ENET_MMFR & 0xFFFFu);
}
void enet_mdio_write(uint8_t phy, uint8_t reg, uint16_t val) {
    ENET_EIR = ENET_EIR_MII;
    ENET_MMFR = ENET_MMFR_ST_01 | ENET_MMFR_OP_WRITE | ENET_MMFR_PA(phy)
              | ENET_MMFR_RA(reg) | ENET_MMFR_TA_10 | val;
    uint32_t spin = 0;
    while (!(ENET_EIR & ENET_EIR_MII)) { if (++spin > 1000000u) return; }
    ENET_EIR = ENET_EIR_MII;
}
/* Generic clause-22 bring-up, PHY at MDIO addr 3 (RTL8201 on the EVKB).
   No vendor pages, no OUI check -- Zephyr uses the same generic driver for this PHY. */
int enet_phy_link_up(uint32_t timeout_ms) {
    const uint8_t A = 3;
    enet_mdio_write(A, 0, 0x8000);           /* BMCR soft reset */
    uint32_t s = 0; while ((enet_mdio_read(A, 0) & 0x8000) && ++s < 100000u) { }
    enet_mdio_write(A, 4, 0x01E1);           /* ANAR: advertise 100F/100H/10F/10H + 802.3 */
    enet_mdio_write(A, 0, 0x1200);           /* BMCR: autoneg enable + restart */
    uint32_t t0 = millis();
    do {
        (void)enet_mdio_read(A, 1);          /* BMSR latches low: read twice */
        if (enet_mdio_read(A, 1) & 0x0004) return 1;   /* link up */
        delay(5);
    } while ((millis() - t0) < timeout_ms);
    return 0;
}
