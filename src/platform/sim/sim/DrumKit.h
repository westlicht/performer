#pragma once

#include "Audio.h"

namespace sim {

class DrumKit {
public:
    DrumKit(Audio &audio);

    void loadSample(const std::string &filename);

    void trigger(int index);

private:

    Audio &_audio;
    std::vector<Sample::Ptr> _samples;
};

} // namespace sim
