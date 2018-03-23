#pragma once

#include "SystemConfig.h"

#include "sim/Simulator.h"

#include <cstdint>
#include <cstdlib>

class Dac {
public:
    static constexpr int Channels = CONFIG_DAC_CHANNELS;

    typedef uint16_t Value;

    Dac() :
        _simulator(sim::Simulator::instance())
    {}

    void init() {}

    void setValue(int channel, Value value) {
        _values[channel] = value;
    }

    void write(int channel) {
        _simulator.writeDac(channel, _values[channel]);
    }

    void write() {
        for (int channel = 0; channel < Channels; ++channel) {
            write(channel);
        }
    }

private:
    sim::Simulator &_simulator;
    Value _values[Channels];
};
