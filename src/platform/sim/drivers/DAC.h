#pragma once

#include "sim/Simulator.h"

#include <array>

class DAC {
public:
    typedef uint16_t Value;
    typedef std::array<Value, 8> Values;

    DAC(sim::Simulator &simulator) :
        _simulator(simulator)
    {}

    Values &values() { return _values; }
    inline Value &operator()(int channel) { return _values[channel]; }

    void write(int channel) {
        _simulator.writeDAC(channel, _values[channel]);
    }

    void write() {
        for (size_t channel = 0; channel < _values.size(); ++channel) {
            write(channel);
        }
    }

private:
    sim::Simulator &_simulator;
    Values _values;
};
