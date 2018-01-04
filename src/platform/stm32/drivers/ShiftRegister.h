#pragma once

#include "SystemConfig.h"

#include <array>

#include <cstdint>

class ShiftRegister {
public:
    static constexpr size_t NumRegisters = CONFIG_NUM_SR;

    ShiftRegister();

    void init();

    void process();

    uint8_t read(size_t index) const { return _inputs[index]; }
    void write(size_t index, uint8_t value) { _outputs[index] = value; }

private:
    std::array<uint8_t, NumRegisters> _outputs;
    std::array<uint8_t, NumRegisters> _inputs;
};
