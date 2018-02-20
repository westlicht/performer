#pragma once

#include "SystemConfig.h"

#include <array>

#include <cstdint>

class ShiftRegister {
public:
    static constexpr int NumRegisters = CONFIG_NUM_SR;

    ShiftRegister();

    void init();

    void process();

    uint8_t read(int index) const { return _inputs[index]; }
    void write(int index, uint8_t value) { _outputs[index] = value; }

private:
    std::array<uint8_t, NumRegisters> _outputs;
    std::array<uint8_t, NumRegisters> _inputs;
};
