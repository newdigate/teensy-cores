/* NXP MIMXRT1170-EVKB - i.MX RT1176, 16 MB QSPI NOR on FlexSPI1.
 * FlexSPI1 XIP window is at 0x30000000 (RT1176 RM table 2-1).
 * ITCM/DTCM MEMORY lengths below are the fuse-default routing window; ResetHandler
 * reconfigures FlexRAM (up to ~480K DTCM). OCRAM is 512 KB @ 0x20240000.
 * ERAM points at the board's 64 MB SDRAM on SEMC @ 0x80000000. */
MEMORY
{
	ITCM (rx):  ORIGIN = 0x00000000, LENGTH = 256K
	DTCM (rw):  ORIGIN = 0x20000000, LENGTH = 256K
	OCRAM (rw): ORIGIN = 0x20240000, LENGTH = 512K
	FLASH (rx): ORIGIN = 0x30000000, LENGTH = 16384K
	ERAM (rw):  ORIGIN = 0x80000000, LENGTH = 65536K
}

ENTRY(ImageVectorTable)

SECTIONS
{
	.text.headers : {
		. = ORIGIN(FLASH) + 0x400;
		KEEP(*(.flashconfig))
		FILL(0xFF)
		. = ORIGIN(FLASH) + 0x1000;
		KEEP(*(.ivt))
		KEEP(*(.bootdata))
		. = ORIGIN(FLASH) + 0x2000;
	} > FLASH

	.text.code : {
		KEEP(*(.vectors))
		KEEP(*(.startup))
		*(.flashmem*)
		. = ALIGN(4);
		KEEP(*(.init))
		__preinit_array_start = .;
		KEEP (*(.preinit_array))
		__preinit_array_end = .;
		__init_array_start = .;
		KEEP (*(.init_array))
		__init_array_end = .;
		. = ALIGN(4);
	} > FLASH

	.text.progmem : {
		*(.progmem*)
		. = ALIGN(4);
	} > FLASH

	.text.itcm : {
		. = . + 32; /* MPU to trap NULL pointer deref */
		*(.fastrun)
		*(.text*)
		. = ALIGN(16);
	} > ITCM  AT> FLASH

	.ARM.exidx : {
		__exidx_start = .;
		*(.ARM.exidx* .ARM.extab.text* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	} > ITCM  AT> FLASH

	.data : {
		*(.endpoint_queue)   
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.data*)))
	} > DTCM  AT> FLASH

	.vectorsram (NOLOAD) : {
		. = ALIGN(1024);
		KEEP(*(.vectorsram))
		. = ALIGN(4);
	} > DTCM

	.bss ALIGN(4) : {
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.bss*)))
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(COMMON)))
		. = ALIGN(32);
		. = . + 32; /* MPU to trap stack overflow */
	} > DTCM

	.bss.dma (NOLOAD) : {
		*(.hab_log)
		*(.dmabuffers)
		. = ALIGN(32);
	} > OCRAM

	.bss.extram (NOLOAD) : {
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.externalram)))
		. = ALIGN(32);
	} > ERAM

	.text.csf : {
		FILL(0xFF)
		. = ALIGN(1024);
		KEEP(*(.csf))
		__text_csf_end = .;
	} > FLASH

	_stext = ADDR(.text.itcm);
	_etext = ADDR(.text.itcm) + SIZEOF(.text.itcm) + SIZEOF(.ARM.exidx);
	_stextload = LOADADDR(.text.itcm);

	_sdata = ADDR(.data);
	_edata = ADDR(.data) + SIZEOF(.data);
	_sdataload = LOADADDR(.data);

	_sbss = ADDR(.bss);
	_ebss = ADDR(.bss) + SIZEOF(.bss);

	_heap_start = ADDR(.bss.dma) + SIZEOF(.bss.dma);
	_heap_end = ORIGIN(OCRAM) + LENGTH(OCRAM) - 0x8000;  /* reserve top 32K of OCRAM (HAB log / DMA / future use) */

	_extram_start = ADDR(.bss.extram);
	_extram_end = ADDR(.bss.extram) + SIZEOF(.bss.extram);

	_itcm_block_count = (SIZEOF(.text.itcm) + SIZEOF(.ARM.exidx) + 0x7FFF) >> 15;
	/* RT1176 GPR17 FLEXRAM_BANK_CFG: 2 bits/bank, 01=ITCM 10=DTCM 00=OCRAM.
	 * Low _itcm_block_count banks -> ITCM (01); the rest -> DTCM (10).
	 * (The teensy4/RT1062 form `0xAAAAAAAA | ((1<<(n*2))-1)` yields 0b11=reserved
	 * for the ITCM banks and does NOT work on RT1176.) */
	_flexram_bank_config = (0xAAAAAAAA & ~((1 << (_itcm_block_count * 2)) - 1)) | (0x55555555 & ((1 << (_itcm_block_count * 2)) - 1));
	/* Stack at the top of DTCM.  Valid because ResetHandler configures FlexRAM
	 * (GPR17/16/14) BEFORE setting SP, so DTCM-top is backed before any push.
	 * DTCM = (16 - _itcm_block_count) banks x 32K; for 1 ITCM bank _estack = 0x20078000. */
	_estack = ORIGIN(DTCM) + ((16 - _itcm_block_count) << 15);

	_flashimagelen = __text_csf_end - ORIGIN(FLASH);
	_teensy_model_identifier = 0x25;

	.debug_info     0 : { *(.debug_info) }
	.debug_abbrev   0 : { *(.debug_abbrev) }
	.debug_line     0 : { *(.debug_line) }
	.debug_frame    0 : { *(.debug_frame) }
	.debug_str      0 : { *(.debug_str) }
	.debug_loc      0 : { *(.debug_loc) }

}
