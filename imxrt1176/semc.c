/* Teensyduino Core Library — RT1176 EVKB SEMC SDRAM bring-up
 * http://www.pjrc.com/teensy/
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* ============================================================================
 * semc.c — SEMC SDRAM controller init for the MIMXRT1170-EVKB.
 *
 * Brings up the 64 MB SDRAM (two 16-bit chips -> 32-bit port) at CS0 /
 * 0x80000000 so the .externalram (EXTMEM) region is live before C++ ctors run.
 * Called from ResetHandler() (startup.c) after the clocks are up and before
 * __libc_init_array().
 *
 * This is a faithful, single-config port of the NXP MCUXpresso SDK:
 *   - SEMC_Init()            (fsl_semc.c) — MCR reset/enable + queue weights
 *   - SEMC_ConfigureSDRAM()  (fsl_semc.c) — BR/SDRAMCRx register programming,
 *                                           ns->cycle timing, IP init sequence
 *   - SEMC_SendIPCommand()   (fsl_semc.c) — IP command interface
 * with the exact board config from
 *   _boards/evkbmimxrt1170/driver_examples/semc/sdram/cm7/hardware_init.c
 *   (BOARD_InitSEMC), i.e. the ISSI 2x16-bit SDRAM on the EVKB.
 *
 * Config (verbatim from the board example, this chip):
 *   CS0, addr 0x80000000, 64 MB, portSize 32-bit, burstLen 8, column 9-bit,
 *   CAS latency 3, delayChain 2, 8 auto-refresh cycles at init.
 *   Timings (ns): tRP 15, tRCD 15, tRFC/tXSR 70, tWR 10, tCKEoff 42, tRAS 40,
 *   tRRD 10, refresh2refresh 60, selfRefRecovery 70; refreshPeriod 64ms/8192.
 *
 * Timing math is computed against SEMC_CLOCK_HZ (198 MHz = SYS_PLL2_PFD1 594 /3)
 * via the ported SEMC_ConvertTiming() helper — DO NOT hand-tune the cycle
 * counts; a wrong value works in QEMU (no timing model) but corrupts on silicon.
 *
 * CLOCK: semc_sdram_init step 1 routes the SEMC clock root (CLOCK_ROOT4) to
 * SYS_PLL2_PFD1/3 and ungates the SEMC LPCG.  SYS_PLL2 and its PFDs are brought
 * up by the boot ROM (HW-verified: PFD1 reads FRAC=16 -> 594 MHz, ungated; the
 * SD card likewise rides SYS_PLL2_PFD2), so no explicit PLL bring-up is needed —
 * and must NOT be added, as re-initing SYS_PLL2 would disturb the SD/audio PFDs.
 * (set_arm_clock_rt1176 only touches the ARM PLL; the boot header has no DCD.)
 * QEMU models the clock tree as survival, so this is transparent there.
 * ========================================================================== */

#include <stdint.h>
#include "imxrt1176.h"

/* ----------------------------------------------------------------------------
 * SEMC_ConvertTiming — nanoseconds -> SEMC bus cycles, HW-encoded as (cycles-1).
 *
 * Verbatim port of fsl_semc.c::SEMC_ConvertTiming: picosecond resolution to
 * avoid narrowing/overflow, ceiling division, subtract 1 for the HW encoding,
 * saturate to 8 bits.  With SEMC_CLOCK_HZ = 198 MHz (tClk = 5050 ps) this yields
 * for this config: tRP/tRCD->2, tRFC/tSRR->13, tWR->1, tCKEoff->8, tRAS->7,
 * tRRD->1, ref2ref->11.
 * -------------------------------------------------------------------------- */
static uint8_t semc_convert_timing(uint32_t time_ns, uint32_t clkSrc_Hz)
{
	uint64_t tClk_ps = 1000000000000ULL / (uint64_t)clkSrc_Hz;
	uint64_t time_ps = (uint64_t)time_ns * 1000ULL;
	uint64_t requiredCycles;

	if ((time_ns == 0u) || (tClk_ps == 0ULL)) {
		return 0u;
	}

	/* Ceiling(time_ps / tClk_ps). */
	requiredCycles = (time_ps + tClk_ps - 1ULL) / tClk_ps;
	if (requiredCycles == 0ULL) {
		return 0u;
	}

	/* HW encoding is (cycles - 1); saturate to avoid uint8_t wrap. */
	requiredCycles -= 1ULL;
	if (requiredCycles > 255ULL) {
		requiredCycles = 255ULL;
	}
	return (uint8_t)requiredCycles;
}

/* ----------------------------------------------------------------------------
 * semc_send_ipcmd — issue one SEMC IP command to the SDRAM and wait for done.
 *
 * Port of fsl_semc.c::SEMC_SendIPCommand for the SDRAM no-read cases used at
 * init (Precharge-all, Auto-Refresh, Mode-Set).  Mode-Set is a "write" command
 * whose payload goes through IPTXDAT (has_txdata=1); the others carry no data.
 * The INTR.IPCMDDONE poll is bounded (HW is the arbiter — proceed on timeout,
 * same pattern as the USB PLL-lock spins) and write-1-cleared afterwards.
 * -------------------------------------------------------------------------- */
static void semc_send_ipcmd(uint32_t address, uint32_t command, uint32_t txdata, int has_txdata)
{
	volatile uint32_t guard;

	/* Clear the done status, then latch the address. */
	SEMC_INTR = SEMC_INTR_IPCMDDONE_MASK;
	SEMC_IPCR0 = address;

	if (has_txdata) {
		SEMC_IPTXDAT = txdata;
	}

	/* Issue command code | magic key. */
	SEMC_IPCMD = (command & SEMC_IPCMD_CMD_MASK) | SEMC_IPCMD_KEY(SEMC_IPCMD_KEY_MAGIC);

	/* Bounded wait for command completion. */
	for (guard = 0; guard < 100000u &&
	     (SEMC_INTR & SEMC_INTR_IPCMDDONE_MASK) == 0u; guard++) {
		/* spin */
	}
	SEMC_INTR = SEMC_INTR_IPCMDDONE_MASK;   /* write-1-clear */
}

/* ----------------------------------------------------------------------------
 * semc_sdram_init — configure SEMC + the 64 MB CS0 SDRAM at 0x80000000.
 * -------------------------------------------------------------------------- */
void semc_sdram_init(void)
{
	volatile uint32_t guard;
	int i;
	uint32_t timing;

	/* ==== 1. Clock =========================================================
	 * Route the SEMC clock root (CLOCK_ROOT4 / kCLOCK_Root_Semc) to
	 * SYS_PLL2_PFD1 (594 MHz) with divide-by-3 -> 198 MHz == SEMC_CLOCK_HZ.
	 * MUX=6 (kCLOCK_SEMC_ClockRoot_MuxSysPll2Pfd1), DIV field = 2 (divide by 3).
	 * This matches BOARD_BootClockRUN (CLOCK_SetRootClock div=3) and the DCD
	 * UpdateSemcClock() value 0x602.  NB: requires SYS_PLL2 + PFD1 powered by the
	 * clock task (see file header) — harmless/idempotent otherwise.
	 */
	CCM_CLOCK_ROOT4_CONTROL = CCM_CLOCK_ROOT_CONTROL_MUX(6) | CCM_CLOCK_ROOT_CONTROL_DIV(2);
	__asm__ volatile("dsb" ::: "memory");

	/* Ungate the SEMC peripheral clock (LPCG33 / kCLOCK_Semc). */
	CCM_LPCG33_DIRECT = 1u;
	__asm__ volatile("dsb" ::: "memory");

	/* ==== 2. Pins ==========================================================
	 * Program the 58 SDRAM SEMC signals: mux = ALT0, pad-ctl = SEMC_PAD_CTL_VAL
	 * (high drive strength + internal pulldown, 0x08).  The 63 GPIO_EMC mux and
	 * pad-ctl registers form two contiguous blocks (Task-1 header), so index
	 * from the B1_00 base pointers:
	 *   idx  0..39 -> GPIO_EMC_B1_00..B1_39  (DATA00-15, DM00/DM01, ADDR00-12,
	 *                                          BA0/BA1, CAS, RAS, CLK, CKE, WE,
	 *                                          CS0, DQS)
	 *   idx 42..59 -> GPIO_EMC_B2_00..B2_17  (DATA16-31, DM02, DM03)
	 * Skip idx 40/41 (RDY, CSX00) and stop before idx 60..62 (DQS4, CLKX00/01):
	 * none are SDRAM signals.  DQS (B1_39, idx 39) additionally gets SION (force
	 * input path on) so the DQS-pad read-strobe loopback works (MCR.DQSMD=1) —
	 * matching the cm7 SDK pin_mux, which sets "Software Input On Field" on DQS.
	 */
	{
		volatile uint32_t *mux = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_B1_00; /* 0x400E8010 */
		volatile uint32_t *pad = &IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_B1_00; /* 0x400E8254 */
		for (i = 0; i <= 59; i++) {
			if (i == 40 || i == 41) {
				continue;   /* RDY, CSX00 — not SDRAM */
			}
			/* DQS pad (idx 39) needs SION (bit 4 = 0x10) to force its input
			 * path on for the read-strobe loopback; other SDRAM pads are ALT0. */
			mux[i] = (i == 39) ? (SEMC_PAD_MUX_ALT | 0x10u) : SEMC_PAD_MUX_ALT;
			pad[i] = SEMC_PAD_CTL_VAL;   /* 0x08 */
		}
	}
	__asm__ volatile("dsb" ::: "memory");

	/* ==== 3. Controller (SEMC_Init) =======================================
	 * Zero all base regions, software-reset the SEMC core, then configure.
	 */
	for (i = 0; i < 9; i++) {
		SEMC_BR(i) = 0u;
	}

	SEMC_MCR = SEMC_MCR_SWRST_MASK;
	for (guard = 0; guard < 100000u && (SEMC_MCR & SEMC_MCR_SWRST_MASK) != 0u; guard++) {
		/* bounded wait for self-clearing software reset */
	}

	/* Disable the module while configuring.  Driver defaults from
	 * SEMC_GetDefaultConfig: busTimeoutCycles=0x1F, cmdTimeoutCycles=0xFF.
	 * DQSMD = kSEMC_Loopbackdqspad (1): the EVKB routes the SDRAM read strobe
	 * through the DQS-pad loopback (the cm7 SDK uses it "for more accurate
	 * timing"); the DQS pad (B1_39) has SION forced on above. */
	SEMC_MCR |= SEMC_MCR_MDIS_MASK |
	            SEMC_MCR_BTO(0x1Fu) |
	            SEMC_MCR_CTO(0xFFu) |
	            SEMC_MCR_DQSMD(1u);

	/* AXI command-queue arbitration weights — SEMC_GetDefaultConfig typical
	 * values (queue A -> BMCR0, queue B -> BMCR1), constructed via the field
	 * macros exactly as the driver's weight union packs them:
	 *   BMCR0 = WQOS 5, WAGE 8, WSH 0x40, WRWS 0x10           (-> 0x00104085)
	 *   BMCR1 = WQOS 5, WAGE 8, WPH 0x60, WRWS 0x24, WBR 0x40 (-> 0x40246085) */
	SEMC_BMCR0 = SEMC_BMCR0_WQOS(5u) | SEMC_BMCR0_WAGE(8u) |
	             SEMC_BMCR0_WSH(0x40u) | SEMC_BMCR0_WRWS(0x10u);
	SEMC_BMCR1 = SEMC_BMCR1_WQOS(5u) | SEMC_BMCR1_WAGE(8u) |
	             SEMC_BMCR1_WPH(0x60u) | SEMC_BMCR1_WRWS(0x24u) | SEMC_BMCR1_WBR(0x40u);

	/* Enable the SEMC module. */
	SEMC_MCR &= ~SEMC_MCR_MDIS_MASK;
	__asm__ volatile("dsb" ::: "memory");

	/* ==== 3b. SDRAM controller (SEMC_ConfigureSDRAM) ======================= */

	/* Base region 0: base 0x80000000, MS = 14 (64 MB), valid.
	 * SEMC_CovertMemorySize(65536 KB): 65536/8 = 8192 = 2^13 -> 14 shifts to 0
	 * -> MS field = 14.  BR0 = 0x8000001D. */
	SEMC_BR(0) = (0x80000000u & SEMC_BR_BA_MASK) | SEMC_BR_MS(14u) | SEMC_BR_VLD_MASK;

	/* SDRAMCR0: portSize 32-bit(2), burstLen 8(3), column 9-bit(3), CAS 3(3).
	 * (columnAddrBitNum = kSEMC_SdramColunm_9bit uses the COL field, not COL8.) */
	SEMC_SDRAMCR0 = SEMC_SDRAMCR0_PS(2u) | SEMC_SDRAMCR0_BL(3u) |
	                SEMC_SDRAMCR0_COL(3u) | SEMC_SDRAMCR0_CL(3u);

	/* CS0: no CSX IOMUX remap (that path is only for cs != CS0).  Clear the
	 * SEMC-internal A8 mux, per SEMC_ConfigureSDRAM. */
	SEMC_IOCR &= ~SEMC_IOCR_MUX_A8_MASK;

	/* Delay chain: SDRAMVAL = delayChain-1 = 5 (delayChain = 6, the cm7 SDK value
	 * "for all temperatures"; cm4 uses 2), SDRAMEN = 1. */
	SEMC_DCCR = (SEMC_DCCR & ~(SEMC_DCCR_SDRAMVAL_MASK | SEMC_DCCR_SDRAMEN_MASK)) |
	            SEMC_DCCR_SDRAMVAL(5u) | SEMC_DCCR_SDRAMEN_MASK;

	/* SDRAMCR1 timings (ns -> cycles @ SEMC_CLOCK_HZ). */
	timing  = SEMC_SDRAMCR1_PRE2ACT(semc_convert_timing(15u, SEMC_CLOCK_HZ)); /* tRP  */
	timing |= SEMC_SDRAMCR1_ACT2RW (semc_convert_timing(15u, SEMC_CLOCK_HZ)); /* tRCD */
	timing |= SEMC_SDRAMCR1_RFRC   (semc_convert_timing(70u, SEMC_CLOCK_HZ)); /* tRFC/tXSR */
	timing |= SEMC_SDRAMCR1_WRC    (semc_convert_timing(10u, SEMC_CLOCK_HZ)); /* tWR  */
	timing |= SEMC_SDRAMCR1_CKEOFF (semc_convert_timing(42u, SEMC_CLOCK_HZ)); /* tCKEoff */
	timing |= SEMC_SDRAMCR1_ACT2PRE(semc_convert_timing(40u, SEMC_CLOCK_HZ)); /* tRAS */
	SEMC_SDRAMCR1 = timing;

	/* SDRAMCR2 timings.  tIdleTimeout_Ns = 0 -> ITO = 0. */
	timing  = SEMC_SDRAMCR2_SRRC   (semc_convert_timing(70u, SEMC_CLOCK_HZ)); /* selfRefRecovery */
	timing |= SEMC_SDRAMCR2_REF2REF(semc_convert_timing(60u, SEMC_CLOCK_HZ));
	timing |= SEMC_SDRAMCR2_ACT2ACT(semc_convert_timing(10u, SEMC_CLOCK_HZ)); /* tRRD */
	timing |= SEMC_SDRAMCR2_ITO(0u);
	SEMC_SDRAMCR2 = timing;

	/* SDRAMCR3 refresh timing — integer math exactly per SEMC_ConfigureSDRAM.
	 *   tPrescalePeriod_Ns   = 160 * (1e9 / clk)               = 800 ns
	 *   prescale             = tPrescale / 16 / (1e9 / clk)    = 10
	 *   refreshPeriod_nsRow  = 64ms / 8192                     = 7812 ns
	 *   refresh              = refreshPeriod / tPrescale       = 9
	 *   urgentRef            = refreshUrgThreshold / tPrescale = 9
	 *   refreshBurstLen = 1  -> REBL = 0. */
	{
		uint32_t clk               = SEMC_CLOCK_HZ;
		uint32_t tPrescalePeriodNs  = 160u * (1000000000u / clk);
		uint32_t refreshPeriodNsRow = 64u * 1000000u / 8192u;
		uint32_t prescale = tPrescalePeriodNs / 16u / (1000000000u / clk);
		uint32_t refresh  = refreshPeriodNsRow / tPrescalePeriodNs;
		uint32_t urgent   = refreshPeriodNsRow / tPrescalePeriodNs; /* refreshUrgThreshold == refreshPeriod */

		SEMC_SDRAMCR3 = SEMC_SDRAMCR3_REBL(1u - 1u) |
		                SEMC_SDRAMCR3_PRESCALE(prescale) |
		                SEMC_SDRAMCR3_RT(refresh - 1u) |
		                SEMC_SDRAMCR3_UT(urgent);
	}

	/* IP command interface: address window + 2-byte transfer size. */
	SEMC_IPCR0 = 0x80000000u;
	SEMC_IPCR1 = 2u;
	SEMC_IPCR2 = 0u;
	__asm__ volatile("dsb" ::: "memory");

	/* ==== 4. SDRAM power-up IP sequence ====================================
	 * Precharge-all -> 8x Auto-Refresh -> Mode-Set, then enable auto-refresh.
	 * Mode register value = burstLen(8)=3 | (casLatency(3) << 4) = 0x33. */
	semc_send_ipcmd(0x80000000u, (uint32_t)kSEMC_SDRAMCM_Prechargeall, 0u, 0);

	for (i = 0; i < 8; i++) {   /* autofreshTimes = 8 */
		semc_send_ipcmd(0x80000000u, (uint32_t)kSEMC_SDRAMCM_AutoRefresh, 0u, 0);
	}

	{
		uint32_t mode = (uint32_t)3u | ((uint32_t)3u << 4);  /* BL8 | (CL3 << MODESETCAL_OFFSET(4)) */
		semc_send_ipcmd(0x80000000u, (uint32_t)kSEMC_SDRAMCM_Modeset, mode, 1);
	}

	/* Enable refresh. */
	SEMC_SDRAMCR3 |= SEMC_SDRAMCR3_REN_MASK;
	__asm__ volatile("dsb" ::: "memory");
}
