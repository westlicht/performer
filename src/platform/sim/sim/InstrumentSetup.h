#pragma once

#include "instruments/DrumSampler.h"
#include "instruments/Synth.h"

#include <memory>

namespace sim {

class InstrumentSetup {
public:
    virtual ~InstrumentSetup() {}
    
    virtual void setGate(int channel, bool gate) {
        _instruments[channel]->setGate(gate);
    }

    virtual void setCv(int channel, float cv) {
        _instruments[channel]->setCv(cv);
    }

protected:
    std::vector<Instrument::Ptr> _instruments;
};

class SamplerSetup : public InstrumentSetup {
public:
    SamplerSetup(Audio &audio);
};

class MixedSetup : public InstrumentSetup {
public:
    MixedSetup(Audio &audio);
};

} // namespace sim
