#pragma once

#include "../Audio.h"
#include "../Instrument.h"

namespace sim {

class DrumSampler : public Instrument {
public:
    DrumSampler(Audio &audio, const std::string &filename);

    virtual void setGate(bool gate) override;
    virtual void setCv(float cv) override;

private:
    void trigger();

    Audio &_audio;
    Sample _sample;
    bool _gate = false;
};

} // namespace sim
