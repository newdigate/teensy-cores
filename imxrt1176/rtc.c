/* SNVS real-time clock for the MIMXRT1176 (i.MX RT1176, Cortex-M7).
 *
 * Port of cores/teensy4/rtc.c — the RT1176 has the same SNVS block, so the body
 * is identical; only the base address differs (RT1062 0x400D4000 -> RT1176
 * 0x40C90000), supplied by the SNVS_* macros in imxrt1176.h.
 *
 * rtc_get() reads the SNVS_HP RTC (a 47-bit 32.768 kHz up-counter) tear-free and
 * returns Unix epoch seconds. rtc_set() writes the SNVS_LP *secure* RTC and then
 * starts+syncs HP from it (HPCR RTC_EN|HP_TS). The LP secure counter is in the
 * always-on (VBAT) domain, so time survives a warm reset (startup.c re-syncs HP
 * from the still-running LP secure counter on the next boot).
 *
 * Signatures are frozen by core_pins.h (Teensy3Clock) and the SD/FAT timestamp
 * path: unsigned long rtc_get(void) / void rtc_set(unsigned long) /
 * void rtc_compensate(int), all Unix epoch seconds.
 */

#include "imxrt1176.h"
#include <sys/time.h> // for struct timeval

unsigned long rtc_get(void)
{
	uint32_t hi1 = SNVS_HPRTCMR;
	uint32_t lo1 = SNVS_HPRTCLR;
	while (1) {
		uint32_t hi2 = SNVS_HPRTCMR;
		uint32_t lo2 = SNVS_HPRTCLR;
		if (lo1 == lo2 && hi1 == hi2) {
			return (hi2 << 17) | (lo2 >> 15);
		}
		hi1 = hi2;
		lo1 = lo2;
	}
}

void rtc_set(unsigned long t)
{
	// stop the RTC
	SNVS_HPCR &= ~(SNVS_HPCR_RTC_EN | SNVS_HPCR_HP_TS);
	while (SNVS_HPCR & SNVS_HPCR_RTC_EN); // wait
	// stop the SRTC
	SNVS_LPCR &= ~SNVS_LPCR_SRTC_ENV;
	while (SNVS_LPCR & SNVS_LPCR_SRTC_ENV); // wait
	// set the SRTC
	SNVS_LPSRTCLR = t << 15;
	SNVS_LPSRTCMR = t >> 17;
	// start the SRTC
	SNVS_LPCR |= SNVS_LPCR_SRTC_ENV;
	while (!(SNVS_LPCR & SNVS_LPCR_SRTC_ENV)); // wait
	// start the RTC and sync it to the SRTC
	SNVS_HPCR |= SNVS_HPCR_RTC_EN | SNVS_HPCR_HP_TS;
}

void rtc_compensate(int adjust __attribute__((unused)))
{
}

// https://github.com/arduino-libraries/ArduinoBearSSL/issues/54
// https://forum.pjrc.com/threads/70966
__attribute__((weak))
int _gettimeofday(struct timeval *tv, void *ignore __attribute__((unused)))
{
	uint32_t hi1 = SNVS_HPRTCMR;
	uint32_t lo1 = SNVS_HPRTCLR;
	while (1) {
		uint32_t hi2 = SNVS_HPRTCMR;  // ref manual 20.3.3.1.3 page 1231
		uint32_t lo2 = SNVS_HPRTCLR;
		if (lo1 == lo2 && hi1 == hi2) {
			tv->tv_sec = (hi2 << 17) | (lo2 >> 15);
			tv->tv_usec = ((lo2 & 0x7FFF) * 15625) >> 9;
			return 0;
		}
		hi1 = hi2;
		lo1 = lo2;
	}
}
