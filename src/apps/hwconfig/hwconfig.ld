/* Linker script for ST STM32F405RGTx (1024K flash, 192K RAM) */

/* Define memory regions. */
MEMORY
{
	ROM (rx) : ORIGIN = 0x08008000, LENGTH = 16K
}

/* Define sections. */
SECTIONS
{
	.hwconfig : {
        *(.hwconfig*)
        KEEP (*(.hwconfig))
	} >ROM

}
