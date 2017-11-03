#pragma once

// CPU
#define CONFIG_CPU_FREQUENCY            168000000
#define CONFIG_TICK_FREQUENCY           1000

// printf
#define CONFIG_PRINTF_BUFFER            16

// Debugging
#define CONFIG_ENABLE_DEBUG             1
#define CONFIG_ENABLE_PROFILER          0

// USB host
#define CONFIG_ENABLE_USBH_DRIVER_FS    1
#define CONFIG_ENABLE_USBH_DRIVER_HS    1

// Pulses per quarter note
#define CONFIG_PPQN                     192

// LCD
#define CONFIG_LCD_WIDTH                256
#define CONFIG_LCD_HEIGHT               64

// CV/Gate channels
#define CONFIG_CHANNEL_COUNT            8

// Model
#define CONFIG_PATTERN_COUNT            64
#define CONFIG_TRACK_COUNT              8
#define CONFIG_STEP_COUNT               16

// Button Led Matrix
#define CONFIG_BLM_ROWS                 8
#define CONFIG_BLM_COLS_BUTTON          5
#define CONFIG_BLM_COLS_LED             4

#define CONFIG_FUNCTION_KEY_COUNT       5
