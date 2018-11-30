#include "Audio.h"

namespace sim {

// ----------------------------------------------------------------------------
// Audio
// ----------------------------------------------------------------------------

Audio::Audio() {
    _engine.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::AUTO, 44100, 512);
}

Audio::~Audio() {
    _engine.deinit();
}

void Audio::play(Sample &sample) {
    _engine.play(sample._wav);
}

void Audio::stopAll() {
    _engine.stopAll();
}

// ----------------------------------------------------------------------------
// Sample
// ----------------------------------------------------------------------------

Sample::Sample(const std::string &filename) {
    _wav.load(filename.c_str());
}

} // namespace sim
