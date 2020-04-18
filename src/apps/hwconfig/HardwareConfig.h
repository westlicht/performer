#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define HARDWARE_CONFIG_ADDR 0x08008000
#define HARDWARE_CONFIG_MAGIC 0x323f6e10
#define HARDWARE_CONFIG_VERSION 3

enum DacType {
    DAC8568C,
    DAC8568A
};

struct HardwareConfigData {
    uint32_t magic;
    uint32_t version;
    uint8_t reverseEncoder;
    uint8_t invertLeds; // added in version 2
    uint8_t dacType; // added in version 3
};

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

struct HardwareConfig {
    static const HardwareConfigData &data() { return *reinterpret_cast<const HardwareConfigData *>(HARDWARE_CONFIG_ADDR); }
    static bool isValid() { return data().magic == HARDWARE_CONFIG_MAGIC; }
    static bool reverseEncoder() { return isValid() && data().reverseEncoder; }
    static bool invertLeds() { return isValid() && data().version >= 2 && data().invertLeds; }
    static DacType dacType() { return DacType(isValid() && data().version >= 3 ? data().dacType : 0); }
};

#endif
