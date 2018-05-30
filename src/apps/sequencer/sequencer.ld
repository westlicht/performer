/* Linker script for ST STM32F405RGTx (1024K flash, 192K RAM) */

/* Define memory regions. */
MEMORY
{
	ROM (rx) : ORIGIN = 0x08010000, LENGTH = 960K
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
	CCMRAM (rw) : ORIGIN = 0x10000000, LENGTH = 64K
}

/* Enforce emmition of the vector table. */
EXTERN (vector_table)

/* Enforce emmition of the version tag. */
EXTERN (version_tag)

/* Define the entry point of the output file. */
ENTRY(reset_handler)

/* Define sections. */
SECTIONS
{
	.text : {
		*(.vectors)	/* Vector table */
		. = ALIGN(1024);
		*(.version)	/* Version tag */
		. = ALIGN(4);
		*(.text*)	/* Program code */
		. = ALIGN(4);
		*(.rodata*)	/* Read-only data */
		. = ALIGN(4);
	} >ROM

	/* C++ Static constructors/destructors, also used for __attribute__
	 * ((constructor)) and the likes */
	.preinit_array : {
		. = ALIGN(4);
		__preinit_array_start = .;
		KEEP (*(.preinit_array))
		__preinit_array_end = .;
	} >ROM
	.init_array : {
		. = ALIGN(4);
		__init_array_start = .;
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array))
		__init_array_end = .;
	} >ROM
	.fini_array : {
		. = ALIGN(4);
		__fini_array_start = .;
		KEEP (*(.fini_array))
		KEEP (*(SORT(.fini_array.*)))
		__fini_array_end = .;
	} >ROM

	/*
	 * Another section used by C++ stuff, appears when using newlib with
	 * 64bit (long long) printf support
	 */
	.ARM.extab : {
		*(.ARM.extab*)
	} >ROM
	.ARM.exidx : {
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} >ROM

	. = ALIGN(4);
	_etext = .;

	.data : {
		_data = .;
		*(.data*)	/* Read-write initialized data */
		. = ALIGN(4);
		_edata = .;
	} >RAM AT >ROM
	_data_loadaddr = LOADADDR(.data);

	.bss : {
		*(.bss*)	/* Read-write zero initialized data */
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
	} >RAM

	.ccmram_bss (NOLOAD) : {
		. = ALIGN(4);
		_sccmram_bss = .; 	/* create a global symbol at ccmram_bss start */
		*(.ccmram_bss)
		*(.ccmram_bss*)
		. = ALIGN(4);
		_eccmram_bss = .;   /* create a global symbol at ccmram_bss end */
	} >CCMRAM

	/*
	 * The .eh_frame section appears to be used for C++ exception handling.
	 * You may need to fix this if you're using C++.
	 */
	/DISCARD/ : { *(.eh_frame) }

	. = ALIGN(4);
	end = .;
}

PROVIDE(_stack = ORIGIN(RAM) + LENGTH(RAM));
