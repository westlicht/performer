#pragma once

#include "Config.h"

#include "drivers/DAC.h"

class CVOutput {
public:
    static constexpr size_t Channels = CONFIG_CV_OUTPUT_CHANNELS;

    CVOutput(DAC &dac);

    void init();

    void update();

    float channel(int index) const {
        return _channels[index];
    }

    void setChannel(int index, float value) {
        _channels[index] = value;
    }

private:
    DAC &_dac;

    float _channels[Channels];
};
