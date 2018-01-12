/* Linker script for ST STM32F405RGTx (1024K flash, 192K RAM) */

/* Define memory regions. */
MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 192K
}

/* Include the common ld script. */
INCLUDE libopencm3_stm32f4.ld
