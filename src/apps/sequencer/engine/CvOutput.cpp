#include "CvOutput.h"

#include "core/math/Math.h"

CvOutput::CvOutput(Dac &dac, const Calibration &calibration) :
    _dac(dac),
    _calibration(calibration)
{}

void CvOutput::init() {
    _channels.fill(0.f);
}

void CvOutput::update() {
    for (int i = 0; i < Channels; ++i) {
        _dac.setValue(i, _calibration.cvOutput(i).voltsToValue(_channels[i]));
    }
    _dac.write();
}
