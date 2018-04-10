#pragma once

#include "TrackEngine.h"
#include "SequenceState.h"

#include "model/Track.h"

class CurveTrackEngine : public TrackEngine {
public:
    CurveTrackEngine(const Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(track, linkedTrackEngine),
        _curveTrack(track.curveTrack())
    {}

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;
    virtual void update(float dt) override;
    virtual void changePattern() override;

    virtual const TrackLinkData *linkData() const override { return &_linkData; }

    virtual bool activity() const override { return true; }
    virtual bool gateOutput(int index) const override { return !_mute; }
    virtual float cvOutput(int index) const override { return _cvOutput; }

    const CurveSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const CurveSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _currentStep; }
    float currentStepFraction() const { return _currentStepFraction; }

    static constexpr Track::TrackMode trackMode = Track::TrackMode::Curve;

private:
    void updateOutput(uint32_t relativeTick, uint32_t divisor);

    const CurveTrack &_curveTrack;

    TrackLinkData _linkData;

    const CurveSequence *_sequence;
    SequenceState _sequenceState;
    int _currentStep;
    float _currentStepFraction;

    float _cvOutput;
};
