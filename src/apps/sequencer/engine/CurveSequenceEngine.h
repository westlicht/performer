#pragma once

#include "SequenceEngine.h"

#include "model/TrackSetup.h"
#include "model/Sequence.h"

class CurveSequenceEngine : public SequenceEngine {
public:
    virtual void setup(const TrackSetup &trackSetup) override;

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;

    virtual void setSequence(const Sequence &sequence) override;
    virtual void setMute(bool mute) override;

    virtual bool gate() const override { return false; }
    virtual bool gateOutput() const override { return false; }
    virtual float cvOutput() const override { return _cvOutput; }

    const CurveSequence &sequence() const { return *_sequence; }

private:
    const CurveSequence *_sequence;

    float _cvOutput;
};
