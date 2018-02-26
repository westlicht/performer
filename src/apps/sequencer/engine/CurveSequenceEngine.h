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
    virtual void setFill(bool fill) override;

    virtual bool gate() const override { return true; }
    virtual bool gateOutput() const override { return !_mute; }
    virtual float cvOutput() const override { return _cvOutput; }

    const CurveSequence &sequence() const { return *_sequence; }

    int currentStep() const { return _currentStep; }
    float currentStepFraction() const { return _currentStepFraction; }

private:
    void advance();

    const CurveSequence *_sequence;

    bool _mute;
    float _cvOutput;
    int _currentStep;
    float _currentStepFraction;
    int8_t _direction;

    CurveSequence::Range _lastRange;
    std::array<float, 2> _range;
};
