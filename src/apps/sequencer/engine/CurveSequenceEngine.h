#pragma once

#include "SequenceEngine.h"
#include "SequenceState.h"

#include "model/Track.h"
#include "model/Sequence.h"

class CurveSequenceEngine : public SequenceEngine {
public:
    virtual void setup(const Track &track) override;

    virtual void setSequence(const Sequence &sequence) override;

    virtual const SequenceLinkData *sequenceLinkData() const override { return &_sequenceLinkData; }

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;

    virtual bool gate() const override { return true; }
    virtual bool gateOutput() const override { return !_mute; }
    virtual float cvOutput() const override { return _cvOutput; }

    const CurveSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const CurveSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _sequenceState.step(); }
    float currentStepFraction() const { return _stepFraction; }

private:
    void updateOutput(uint32_t relativeTick, uint32_t divisor);

    const Track *_track;
    const CurveSequence *_sequence;

    SequenceState _sequenceState;
    float _stepFraction;
    SequenceLinkData _sequenceLinkData;

    float _cvOutput;

    CurveSequence::Range _lastRange;
    std::array<float, 2> _range;
};
