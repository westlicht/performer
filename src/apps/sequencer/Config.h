#pragma once

#include "SystemConfig.h"

// Task priorities
#define CONFIG_DRIVER_TASK_PRIORITY     5
#define CONFIG_ENGINE_TASK_PRIORITY     4
#define CONFIG_USBH_TASK_PRIORITY       3
#define CONFIG_UI_TASK_PRIORITY         2
#define CONFIG_FILE_TASK_PRIORITY       1
#define CONFIG_PROFILER_TASK_PRIORITY   0

// Parts per quarter note
#define CONFIG_PPQN                     192

// Sequence parts per quarter note resolution
#define CONFIG_SEQUENCE_PPQN            48

// Default UI frames per second
#define CONFIG_DEFAULT_UI_FPS           50

// CV/Gate channels
#define CONFIG_CHANNEL_COUNT            8

// CV inputs
#define CONFIG_CV_INPUT_CHANNELS        4

// CV outputs
#define CONFIG_CV_OUTPUT_CHANNELS       8

// Model
#define CONFIG_PATTERN_COUNT            16
#define CONFIG_TRACK_COUNT              8
#define CONFIG_STEP_COUNT               64
#define CONFIG_ROUTE_COUNT              16
#define CONFIG_USER_SCALE_COUNT         4
#define CONFIG_USER_SCALE_SIZE          32


// #define CONFIG_ENABLE_ASTEROIDS
// #define CONFIG_ENABLE_INTRO
