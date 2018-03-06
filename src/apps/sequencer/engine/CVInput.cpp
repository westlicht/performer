#include "CVInput.h"

CVInput::CVInput(ADC &adc) :
    _adc(adc)
{
}

void CVInput::init() {
    _channels.fill(0.f);
}

void CVInput::update() {
    for (int i = 0; i < Channels; ++i) {
        _channels[i] = _adc.channel(i) / 6553.5f - 5.f;
    }
}
