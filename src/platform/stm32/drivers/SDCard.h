#pragma once

#include "core/Debug.h"

#include <cstddef>
#include <cstdint>

class SDCard {
public:
    void init() {
    }

    bool available() {
        return false;
    }

    bool writeProtected() {
        return false;
    }

    size_t sectorCount() const { return 0; }
    size_t sectorSize() const { return 512; }

    bool read(uint8_t *buf, uint32_t sector, uint8_t count) {
        return false;
    }

    bool write(const uint8_t *buf, uint32_t sector, uint8_t count) {
        return false;
    }

    void sync() {
    }

};
