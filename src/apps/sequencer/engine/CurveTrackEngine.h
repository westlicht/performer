#pragma once

#include "TrackEngine.h"
#include "SequenceState.h"

#include "model/Track.h"

class CurveTrackEngine : public TrackEngine {
public:
    CurveTrackEngine(const Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(track, linkedTrackEngine)
    {}

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;
    virtual void changePattern() override;

    virtual const TrackLinkData *linkData() const override { return &_linkData; }

    virtual bool gate() const override { return true; }
    virtual bool gateOutput() const override { return !_mute; }
    virtual float cvOutput() const override { return _cvOutput; }

    const CurveSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const CurveSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _sequenceState.step(); }
    float currentStepFraction() const { return _stepFraction; }

    static constexpr Track::TrackMode trackMode = Track::TrackMode::Curve;

private:
    void updateOutput(uint32_t relativeTick, uint32_t divisor);

    TrackLinkData _linkData;

    const CurveSequence *_sequence;
    SequenceState _sequenceState;
    float _stepFraction;

    float _cvOutput;

    CurveSequence::Range _lastRange;
    std::array<float, 2> _range;
};
