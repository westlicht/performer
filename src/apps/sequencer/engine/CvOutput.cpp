#include "CvOutput.h"

CvOutput::CvOutput(DAC &dac) :
    _dac(dac)
{}

void CvOutput::init() {
    _channels.fill(0.f);

}

void CvOutput::update() {
    for (int i = 0; i < Channels; ++i) {
        _dac.setValue(i, uint16_t((10.f - _channels[i]) * 3276.75f));

    }
    _dac.write();
}
