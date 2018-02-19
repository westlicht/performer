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

    bool gateOutput() const { return _gateOutput; }
    float cvOutput() const { return _cvOutput; }

private:
    const CurveSequence *_sequence;

    bool _gateOutput;
    float _cvOutput;
};
