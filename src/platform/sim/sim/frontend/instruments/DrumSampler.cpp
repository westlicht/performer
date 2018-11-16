#include "DrumSampler.h"

namespace sim {

DrumSampler::DrumSampler(Audio &audio, const std::string &filename) :
    _audio(audio),
    _sample(filename)
{
}

void DrumSampler::setGate(bool gate) {
    if (gate != _gate) {
        if (gate) {
            trigger();
        }
        _gate = gate;
    }
}

void DrumSampler::setCv(float cv) {
}

void DrumSampler::trigger() {
    _audio.play(_sample);
}

} // namespace sim
