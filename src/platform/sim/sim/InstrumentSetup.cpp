#include "InstrumentSetup.h"

namespace sim {

SamplerSetup::SamplerSetup(Audio &audio) {
    std::string prefix("assets/drumkit/");

    for (const auto &wav : { "kick.wav", "snare.wav", "rim.wav", "clap.wav", "hh1.wav", "hh2.wav", "tom1.wav", "tom2.wav" }) {
        _instruments.emplace_back(std::make_shared<DrumSampler>(audio, prefix + wav));
    }
}

MixedSetup::MixedSetup(Audio &audio) {
    std::string prefix("assets/drumkit/");

    for (const auto &wav : { "kick.wav", "snare.wav", "rim.wav", "clap.wav", "hh1.wav", "hh2.wav", "tom1.wav" }) {
        _instruments.emplace_back(std::make_shared<DrumSampler>(audio, prefix + wav));
    }

    // _instruments.emplace_back(std::make_shared<Synth>(audio));
    _instruments.emplace_back(std::make_shared<Synth>(audio));
}

} // namespace sim
