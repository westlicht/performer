#pragma once

#include "SystemConfig.h"

#include <cstdint>

class Flash {
public:
    static void unlock() {}
    static void lock() {}
    static void eraseSector(uint32_t sector) {}
    static void program(uint32_t address, uint32_t data) {}
};
