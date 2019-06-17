#pragma once

#include "TrackEngine.h"
#include "SequenceState.h"

#include "model/Track.h"

class CurveTrackEngine : public TrackEngine {
public:
    CurveTrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(engine, model, track, linkedTrackEngine),
        _curveTrack(track.curveTrack())
    {
        reset();
    }

    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::Curve; }

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;
    virtual void update(float dt) override;

    virtual void changePattern() override;

    virtual const TrackLinkData *linkData() const override { return &_linkData; }

    virtual bool activity() const override { return false; }
    virtual bool gateOutput(int index) const override { return !mute(); }
    virtual float cvOutput(int index) const override { return _cvOutput; }
    virtual float sequenceProgress() const override {
        return _currentStep < 0 ? 0.f : float(_currentStep - _sequence->firstStep()) / (_sequence->lastStep() - _sequence->firstStep());
    }

    const CurveSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const CurveSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _currentStep; }
    float currentStepFraction() const { return _currentStepFraction; }

private:
    void updateOutput(uint32_t relativeTick, uint32_t divisor);

    const CurveTrack &_curveTrack;

    TrackLinkData _linkData;

    const CurveSequence *_sequence;
    SequenceState _sequenceState;
    int _currentStep;
    float _currentStepFraction;

    float _cvOutput = 0.f;
    float _cvOutputTarget = 0.f;
};
