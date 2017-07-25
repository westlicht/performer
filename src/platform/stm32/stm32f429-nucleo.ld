/* Linker script for ST STM32F429-NUCLEO-144 (STM32F429ZIT6, 2048K flash, 256K RAM). */

/* Define memory regions. */
MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 192K
}

/* Include the common ld script. */
INCLUDE libopencm3_stm32f4.ld
