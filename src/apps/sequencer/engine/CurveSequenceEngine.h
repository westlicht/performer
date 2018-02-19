#pragma once

#include "model/TrackSetup.h"
#include "model/CurveSequence.h"

class CurveSequenceEngine {
public:
    void setup(const TrackSetup &trackSetup);

    const CurveSequence &sequence() const { return *_sequence; }
    void setSequence(const CurveSequence &sequence) { _sequence = &sequence; }

    void reset();

    void tick(uint32_t tick);

    void setMute(bool mute);

    bool gate() const { return false; }
    bool gateOutput() const { return false; }
    float cvOutput() const { return _cvOutput; }

private:
    const CurveSequence *_sequence;

    float _cvOutput;
};
