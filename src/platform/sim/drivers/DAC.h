#pragma once

#include "sim/Simulator.h"

#include <cstdint>

class DAC {
public:
    typedef uint16_t Value;

    DAC(sim::Simulator &simulator) :
        _simulator(simulator)
    {}

    void setValue(int channel, Value value) {
        _values[channel] = value;
    }

    void write(int channel) {
        _simulator.writeDAC(channel, _values[channel]);
    }

    void write() {
        for (size_t channel = 0; channel < 8; ++channel) {
            write(channel);
        }
    }

private:
    sim::Simulator &_simulator;
    Value _values[8];
};
