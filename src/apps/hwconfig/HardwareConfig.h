#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define HARDWARE_CONFIG_ADDR 0x08008000
#define HARDWARE_CONFIG_MAGIC 0x323f6e10
#define HARDWARE_CONFIG_VERSION 1

struct HardwareConfigData {
    uint32_t magic;
    uint32_t version;
    uint8_t reversedEncoder;
};

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

struct HardwareConfig {
    static const HardwareConfigData &data() { return *reinterpret_cast<const HardwareConfigData *>(HARDWARE_CONFIG_ADDR); }
    static bool isValid() { return data().magic == HARDWARE_CONFIG_MAGIC; }
    static bool reversedEncoder() { return isValid() && data().reversedEncoder; }
};

#endif
