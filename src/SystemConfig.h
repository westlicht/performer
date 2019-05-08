#pragma once

#include "Platform.h"

// CPU
#define CONFIG_CPU_FREQUENCY            168000000
#define CONFIG_TICK_FREQUENCY           1000

// Interrupt priorities
#define CONFIG_HIGHRES_IRQ_PRIORITY     (0<<4)
#define CONFIG_CLOCKTIMER_IRQ_PRIORITY  (1<<4)
#define CONFIG_DIO_IRQ_PRIORITY         (2<<4)
#define CONFIG_MIDI_IRQ_PRIORITY        (3<<4)
#define CONFIG_LCD_IRQ_PRIORITY         (4<<4)
#define CONFIG_CONSOLE_IRQ_PRIORITY     (5<<4)

// printf
#define CONFIG_PRINTF_BUFFER            16

// Debugging
#define CONFIG_ENABLE_DEBUG             1
#define CONFIG_ENABLE_PROFILER          0
#define CONFIG_ENABLE_TASK_PROFILER     1

// Sanitization
#define CONFIG_ENABLE_SANITIZE          1

// USB host
#define CONFIG_ENABLE_USBH_DRIVER_FS    1
#define CONFIG_ENABLE_USBH_DEBUG        0

// LCD
#define CONFIG_LCD_WIDTH                256
#define CONFIG_LCD_HEIGHT               64

// Shift registers
#define CONFIG_NUM_SR                   3

// Button Led Matrix
#define CONFIG_BLM_ROWS                 8
#define CONFIG_BLM_COLS_BUTTON          5
#define CONFIG_BLM_COLS_LED             4

#define CONFIG_FUNCTION_KEY_COUNT       5

// ADC
#define CONFIG_ADC_CHANNELS             4

// DAC
#define CONFIG_DAC_CHANNELS             8

// SdCard
#define CONFIG_SDCARD_USE_CARD_DETECT   1
