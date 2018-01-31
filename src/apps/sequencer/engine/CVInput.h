#pragma once

#include "Config.h"

#include "drivers/ADC.h"

class CVInput {
public:
    static constexpr size_t Channels = CONFIG_CV_INPUT_CHANNELS;

    CVInput(ADC &adc);

    void init();

    void update();

    float channel(int index) const {
        return _channels[index];
    }

private:
    ADC &_adc;

    float _channels[Channels];
};
