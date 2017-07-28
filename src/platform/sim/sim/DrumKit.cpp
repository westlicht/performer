#include "DrumKit.h"

namespace sim {

DrumKit::DrumKit(Audio &audio) :
    _audio(audio)
{
    std::string prefix("assets/drumkit/");
    for (const auto &wav : { "kick.wav", "snare.wav", "rim.wav", "clap.wav", "hh1.wav", "hh2.wav", "tom1.wav", "tom2.wav" }) {
        loadSample(prefix + wav);
    }
}

void DrumKit::loadSample(const std::string &filename) {
    _samples.emplace_back(std::make_shared<Sample>(filename));
}

void DrumKit::trigger(int index) {
    if (index >= 0 && index < _samples.size()) {
        _audio.play(*_samples[index]);
    }
}

} // namespace sim
