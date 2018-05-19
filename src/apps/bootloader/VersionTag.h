#pragma once

#include "Config.h"

#include <cstdint>

struct VersionTag {
    uint32_t magic;
    char name[24];
    uint8_t major;
    uint8_t minor;
    uint16_t revision;

    bool isValid() const {
        return magic == CONFIG_VERSION_TAG_MAGIC;
    }
};
