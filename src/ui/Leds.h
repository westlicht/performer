#pragma once

#include "Config.h"

#include "core/Debug.h"

#include <array>

#include <cstdint>

class Leds {
public:
    typedef std::array<std::pair<uint8_t, uint8_t>, CONFIG_BLM_ROWS * CONFIG_BLM_COLS_LED> LedArray;

    void set(int index, uint8_t red, uint8_t green) {
        ASSERT(index >= 0 && index < int(_array.size()), "invalid led index");
        _array[index] = { red, green };
    }

    const LedArray &array() const { return _array; }

private:
    LedArray _array;
};
