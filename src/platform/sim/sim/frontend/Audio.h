#pragma once

#include "Common.h"

#include "soloud.h"
#include "soloud_wav.h"

namespace sim {

class Sample;

class Audio {
public:
    Audio();
    ~Audio();

    SoLoud::Soloud &engine() { return _engine; }

    void play(Sample &sample);
    void stopAll();

private:
    SoLoud::Soloud _engine;
};

class Sample {
public:
    typedef std::shared_ptr<Sample> Ptr;

    Sample(const std::string &filename);

private:
    SoLoud::Wav _wav;

    friend class Audio;
};

} // namespace sim
