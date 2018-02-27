#pragma once

#include "Config.h"

#include "core/Debug.h"

#include <array>
#include <utility>

#include <cstdint>

class Leds {
public:
    typedef std::array<std::pair<uint8_t, uint8_t>, CONFIG_BLM_ROWS * CONFIG_BLM_COLS_LED> LedArray;

    void clear() {
        _array.fill({ 0, 0 });
    }

    void setDimmed(int index, uint8_t red, uint8_t green) {
        ASSERT(index >= 0 && index < int(_array.size()), "invalid led index");
        _array[index] = { red, green };
    }

    void set(int index, bool red, bool green) {
        setDimmed(index, red ? 0xff : 0, green ? 0xff : 0);
    }

    const LedArray &array() const { return _array; }

private:
    LedArray _array;
};
