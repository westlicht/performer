#pragma once

#include "Config.h"

#include "drivers/ADC.h"

#include <array>

class CvInput {
public:
    static constexpr int Channels = CONFIG_CV_INPUT_CHANNELS;

    CvInput(ADC &adc);

    void init();

    void update();

    float channel(int index) const {
        return _channels[index];
    }

private:
    ADC &_adc;

    std::array<float, Channels> _channels;
};
