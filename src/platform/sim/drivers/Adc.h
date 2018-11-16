#pragma once

#include "SystemConfig.h"

#include "sim/Simulator.h"

#include <array>

#include <cmath>
#include <cstdint>

class Adc : private sim::TargetInputHandler {
public:
    static constexpr int Channels = CONFIG_ADC_CHANNELS;

    Adc() {
        for (int channel = 0; channel < Channels; ++channel) {
            _channels[channel] = 0x7fff;
        }

        sim::Simulator::instance().registerTargetInputObserver(this);
    }

    void init() {}

    uint16_t channel(int index) const {
        return _channels[index];
    }

private:
    void writeAdc(int channel, uint16_t value) override {
        _channels[channel] = value;
    }

    std::array<uint16_t, Channels> _channels;
};
