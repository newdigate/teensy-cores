/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2019 PJRC.COM, LLC.
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

//#include "imxrt1176.h"

#include <stdint.h>

extern void ResetHandler(void);
extern unsigned long _estack;
extern unsigned long _flashimagelen;



__attribute__ ((section(".bootdata"), used))
const uint32_t BootData[3] = {
	0x30000000,		// flash base for NXP MIMXRT1170-EVKB (RT1176)
	(uint32_t)&_flashimagelen,
	0
};


__attribute__ ((section(".csf"), used))
const uint32_t hab_csf[768];	// placeholder for HAB signature


__attribute__ ((section(".ivt"), used))
const uint32_t ImageVectorTable[8] = {
	0x412000D1,		// header: tag=0xD1, length=0x0020, version=0x41
	(uint32_t)&ResetHandler,// program entry
	0,			// reserved
	0,			// dcd (must be 0 for XIP boot)
	(uint32_t)BootData,	// abs address of boot data
	(uint32_t)ImageVectorTable, // self = 0x30001000
	(uint32_t)hab_csf,	// command sequence file
	0			// reserved
};

/*
 * FlexSPI NOR Configuration Block (FCB) for NXP MIMXRT1170-EVKB
 * Flash: ISSI IS25WP128 quad SPI NOR (128 Mbit / 16 MB)
 *
 * These exact bytes are taken from the proven-working Zephyr boot image for
 * the EVKB (build-hello/zephyr/zephyr.bin at offset 0x400, 512 bytes).
 * They have been verified to boot on real hardware and in QEMU with
 * -global fsl-imxrt1170.boot-xip=on.
 *
 * Key FCB fields (little-endian uint32):
 *   [0]  0x42464346  tag 'FCFB'
 *   [1]  0x56010400  version
 *   [3]  0x00030301  readSampleClkSrc=1, dataHoldTime=3, dataSetupTime=3
 *   [7]  0x00000001  configCmdEnable=1
 *   [8]  0x00000C01  configCmdSeq[0]
 *   [12] 0x00000030  cfgCmdArgs[0]
 *   [16] 0x00000010  controllerMiscOption (SafeConfigFreq enabled)
 *   [17] 0x00070401  deviceType=1 (serial NOR), sflashPadType=4 (quad),
 *                    serialClkFreq=7 (133 MHz), lutCustomSeqEnable=0
 *   [20] 0x04000000  sflashA1Size (as used by Zephyr for this board)
 *   LUT entries for quad read (0xEB), status read, write enable, etc.
 *   [112] 0x00010000  pageSize=256
 *   [113] 0x00001000  sectorSize=4096
 *   [114] 0x00000001  ipCmdSerialClkFreq
 *   [116] 0x00010000  blockSize=65536
 */
__attribute__ ((section(".flashconfig"), used))
uint32_t FlexSPI_NOR_Config[128] = {
	/* ---- common FlexSPI configuration block (offset 0x000..0x1BF) ---- */
	0x42464346,	/* [0]   tag 'FCFB'                                    */
	0x56010400,	/* [1]   version 0x56010400                            */
	0x00000000,	/* [2]   reserved                                      */
	0x00030301,	/* [3]   readSampleClkSrc=1,dataHold=3,dataSetup=3     */

	0x00000000,	/* [4]   waitTimeCfgCommands / deviceModeCfgEnable     */
	0x00000000,	/* [5]   deviceModeSeq                                 */
	0x00000000,	/* [6]   deviceModeArg                                 */
	0x00000001,	/* [7]   configCmdEnable=1                             */

	0x00000C01,	/* [8]   configCmdSeqs[0]                              */
	0x00000000,	/* [9]   configCmdSeqs[1]                              */
	0x00000000,	/* [10]  configCmdSeqs[2]                              */
	0x00000000,	/* [11]  configCmdSeqs[3]                              */

	0x00000030,	/* [12]  cfgCmdArgs[0]                                 */
	0x00000000,	/* [13]  cfgCmdArgs[1]                                 */
	0x00000000,	/* [14]  cfgCmdArgs[2]                                 */
	0x00000000,	/* [15]  cfgCmdArgs[3]                                 */

	0x00000010,	/* [16]  controllerMiscOption (SafeConfigFreqEnable)   */
	0x00070401,	/* [17]  deviceType=1,sflashPadType=4,serialClkFreq=7 */
	0x00000000,	/* [18]  reserved                                      */
	0x00000000,	/* [19]  reserved                                      */

	0x04000000,	/* [20]  sflashA1Size (from Zephyr FCB)                */
	0x00000000,	/* [21]  sflashA2Size                                  */
	0x00000000,	/* [22]  sflashB1Size                                  */
	0x00000000,	/* [23]  sflashB2Size                                  */

	0x00000000,	/* [24]  csPadSettingOverride                          */
	0x00000000,	/* [25]  sclkPadSettingOverride                        */
	0x00000000,	/* [26]  dataPadSettingOverride                        */
	0x00000000,	/* [27]  dqsPadSettingOverride                         */

	0x00000000,	/* [28]  timeoutInMs                                   */
	0x00000000,	/* [29]  commandInterval                               */
	0x00000000,	/* [30]  dataValidTime                                 */
	0x00000000,	/* [31]  busyBitPolarity / busyOffset                  */

	/* LUT table (lookupTable[0..63]) */
	0x0A2004EC,	/* [32]  LUT[0]  CMD_SDR(0xEB,1pad) RADDR_SDR(0x20,4pad) */
	0x26043208,	/* [33]  LUT[1]  DUMMY_SDR(8,4pad) READ_SDR(4,4pad)    */
	0x00000000,	/* [34]  LUT[2]                                        */
	0x00000000,	/* [35]  LUT[3]                                        */

	0x24040405,	/* [36]  LUT[4]  (ReadStatus)                          */
	0x00000000,	/* [37]  LUT[5]                                        */
	0x00000000,	/* [38]  LUT[6]                                        */
	0x00000000,	/* [39]  LUT[7]                                        */

	0x00000000,	/* [40]  LUT[8]                                        */
	0x00000000,	/* [41]  LUT[9]                                        */
	0x00000000,	/* [42]  LUT[10]                                       */
	0x00000000,	/* [43]  LUT[11]                                       */

	0x00000406,	/* [44]  LUT[12] (WriteEnable)                         */
	0x00000000,	/* [45]  LUT[13]                                       */
	0x00000000,	/* [46]  LUT[14]                                       */
	0x00000000,	/* [47]  LUT[15]                                       */

	0x00000000,	/* [48]  LUT[16]                                       */
	0x00000000,	/* [49]  LUT[17]                                       */
	0x00000000,	/* [50]  LUT[18]                                       */
	0x00000000,	/* [51]  LUT[19]                                       */

	0x08200421,	/* [52]  LUT[20] (EraseSector)                         */
	0x00000000,	/* [53]  LUT[21]                                       */
	0x00000000,	/* [54]  LUT[22]                                       */
	0x00000000,	/* [55]  LUT[23]                                       */

	0x00000000,	/* [56]  LUT[24]                                       */
	0x00000000,	/* [57]  LUT[25]                                       */
	0x00000000,	/* [58]  LUT[26]                                       */
	0x00000000,	/* [59]  LUT[27]                                       */

	0x00000000,	/* [60]  LUT[28]                                       */
	0x00000000,	/* [61]  LUT[29]                                       */
	0x00000000,	/* [62]  LUT[30]                                       */
	0x00000000,	/* [63]  LUT[31]                                       */

	0x081804D8,	/* [64]  LUT[32] (Read JEDEC ID)                       */
	0x00000000,	/* [65]  LUT[33]                                       */
	0x00000000,	/* [66]  LUT[34]                                       */
	0x00000000,	/* [67]  LUT[35]                                       */

	0x08200412,	/* [68]  LUT[36] (PageProgram)                         */
	0x00002004,	/* [69]  LUT[37]                                       */
	0x00000000,	/* [70]  LUT[38]                                       */
	0x00000000,	/* [71]  LUT[39]                                       */

	0x00000000,	/* [72]  LUT[40]                                       */
	0x00000000,	/* [73]  LUT[41]                                       */
	0x00000000,	/* [74]  LUT[42]                                       */
	0x00000000,	/* [75]  LUT[43]                                       */

	0x00000460,	/* [76]  LUT[44] (ChipErase)                           */
	0x00000000,	/* [77]  LUT[45]                                       */
	0x00000000,	/* [78]  LUT[46]                                       */
	0x00000000,	/* [79]  LUT[47]                                       */

	0x200104C0,	/* [80]  LUT[48]                                       */
	0x00000000,	/* [81]  LUT[49]                                       */
	0x00000000,	/* [82]  LUT[50]                                       */
	0x00000000,	/* [83]  LUT[51]                                       */

	0x00000000,	/* [84]  LUT[52]                                       */
	0x00000000,	/* [85]  LUT[53]                                       */
	0x00000000,	/* [86]  LUT[54]                                       */
	0x00000000,	/* [87]  LUT[55]                                       */

	0x00000000,	/* [88]  LUT[56]                                       */
	0x00000000,	/* [89]  LUT[57]                                       */
	0x00000000,	/* [90]  LUT[58]                                       */
	0x00000000,	/* [91]  LUT[59]                                       */

	0x00000000,	/* [92]  LUT[60]                                       */
	0x00000000,	/* [93]  LUT[61]                                       */
	0x00000000,	/* [94]  LUT[62]                                       */
	0x00000000,	/* [95]  LUT[63]                                       */

	/* LUT custom sequences (lutCustomSeq[0..11]) */
	0x00000000,	/* [96]  lutCustomSeq[0]                               */
	0x00000000,	/* [97]  lutCustomSeq[1]                               */
	0x00000000,	/* [98]  lutCustomSeq[2]                               */
	0x00000000,	/* [99]  lutCustomSeq[3]                               */

	0x00000000,	/* [100] lutCustomSeq[4]                               */
	0x00000000,	/* [101] lutCustomSeq[5]                               */
	0x00000000,	/* [102] lutCustomSeq[6]                               */
	0x00000000,	/* [103] lutCustomSeq[7]                               */

	0x00000000,	/* [104] lutCustomSeq[8]                               */
	0x00000000,	/* [105] lutCustomSeq[9]                               */
	0x00000000,	/* [106] lutCustomSeq[10]                              */
	0x00000000,	/* [107] lutCustomSeq[11]                              */

	/* reserved */
	0x00000000,	/* [108] reserved                                      */
	0x00000000,	/* [109] reserved                                      */
	0x00000000,	/* [110] reserved                                      */
	0x00000000,	/* [111] reserved                                      */

	/* ---- Serial NOR configuration block (offset 0x1C0..0x1FF) ---- */
	0x00010000,	/* [112] pageSize=256 (0x100)                          */
	0x00001000,	/* [113] sectorSize=4096 (0x1000)                      */
	0x00000001,	/* [114] ipCmdSerialClkFreq=1                          */
	0x00000000,	/* [115] reserved                                      */

	0x00010000,	/* [116] blockSize=65536 (0x10000)                     */
	0x00000000,	/* [117] reserved                                      */
	0x00000000,	/* [118] reserved                                      */
	0x00000000,	/* [119] reserved                                      */

	0x00000000,	/* [120] reserved                                      */
	0x00000000,	/* [121] reserved                                      */
	0x00000000,	/* [122] reserved                                      */
	0x00000000,	/* [123] reserved                                      */

	0x00000000,	/* [124] reserved                                      */
	0x00000000,	/* [125] reserved                                      */
	0x00000000,	/* [126] reserved                                      */
	0x00000000,	/* [127] reserved                                      */
};
