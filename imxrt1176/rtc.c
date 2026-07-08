/* Minimal RTC shim for the MIMXRT1176 (i.MX RT1176) core.
 *
 * The Teensy 4 / RT1062 core implements these by reading the SNVS SRTC
 * (see teensy4/rtc.c: SNVS_HPRTCMR / SNVS_HPRTCLR / SNVS_LPSRTC*).  The RT1176
 * SNVS block lives at a different base and is not yet brought up in this core --
 * imxrt1176.h defines no SNVS registers, and QEMU does not model the SNVS RTC.
 *
 * Until an SNVS SRTC port lands, provide the symbols the core header declares
 * (core_pins.h: rtc_get / rtc_set / rtc_compensate, used by Teensy3Clock and any
 * time-aware consumer such as the SD library's FsDateTime callback) so those
 * consumers link.  With no configured clock, rtc_get() returns 0: callers then
 * treat "now" as before 1980 and emit zero FAT timestamps -- exactly the
 * behaviour of a freshly powered board whose RTC was never set.
 *
 * TODO: replace with a real RT1176 SNVS SRTC implementation once the SNVS
 * registers are added to imxrt1176.h and can be hardware-verified.
 */

unsigned long rtc_get(void)
{
	return 0;   // RTC not configured (SNVS SRTC bring-up deferred on RT1176)
}

void rtc_set(unsigned long t __attribute__((unused)))
{
	// no-op until the RT1176 SNVS SRTC is brought up
}

void rtc_compensate(int adjust __attribute__((unused)))
{
	// no-op: no SRTC calibration register configured yet
}
