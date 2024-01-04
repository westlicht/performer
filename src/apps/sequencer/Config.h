#pragma once

#include "SystemConfig.h"

// Version tag
#define CONFIG_VERSION_MAGIC            0xfadebabe
#define CONFIG_VERSION_NAME             "PER|FORMER SEQUENCER"
#define CONFIG_VERSION_MAJOR            0
#define CONFIG_VERSION_MINOR            1
#define CONFIG_VERSION_REVISION         46

// Task priorities
#define CONFIG_DRIVER_TASK_PRIORITY     5
#define CONFIG_ENGINE_TASK_PRIORITY     4
#define CONFIG_USBH_TASK_PRIORITY       3
#define CONFIG_UI_TASK_PRIORITY         2
#define CONFIG_FILE_TASK_PRIORITY       1
#define CONFIG_PROFILER_TASK_PRIORITY   0

// Task stack sizes
#define CONFIG_DRIVER_TASK_STACK_SIZE   1024
#define CONFIG_ENGINE_TASK_STACK_SIZE   4096
#define CONFIG_USBH_TASK_STACK_SIZE     2048
#define CONFIG_UI_TASK_STACK_SIZE       4096
#define CONFIG_FILE_TASK_STACK_SIZE     2048
#define CONFIG_PROFILER_TASK_STACK_SIZE 2048

// Settings flash storage
#define CONFIG_SETTINGS_FLASH_SECTOR    3
#define CONFIG_SETTINGS_FLASH_ADDR      0x0800C000

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
#define CONFIG_SNAPSHOT_COUNT           1
#define CONFIG_SONG_SLOT_COUNT          64
#define CONFIG_TRACK_COUNT              8
#define CONFIG_STEP_COUNT               64
#define CONFIG_ROUTE_COUNT              16
#define CONFIG_MIDI_OUTPUT_COUNT        16
#define CONFIG_USER_SCALE_COUNT         4
#define CONFIG_USER_SCALE_SIZE          32


#define CONFIG_ENABLE_ASTEROIDS
// #define CONFIG_ENABLE_INTRO
