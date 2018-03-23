#include "CvInput.h"

CvInput::CvInput(Adc &adc) :
    _adc(adc)
{
}

void CvInput::init() {
    _channels.fill(0.f);
}

void CvInput::update() {
    for (int i = 0; i < Channels; ++i) {
        _channels[i] = _adc.channel(i) / 6553.5f - 5.f;
    }
}
