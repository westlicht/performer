#pragma once

#include "TrackEngine.h"
#include "SequenceState.h"
#include "SortedQueue.h"
#include "Groove.h"
#include "RecordHistory.h"

class HarmonyTrackEngine : public TrackEngine {
public:
    HarmonyTrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(engine, model, track, linkedTrackEngine),
        _harmonyTrack(track.harmonyTrack())
    {
        reset();
    }

    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::Harmony; }

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;
    virtual void update(float dt) override;

    virtual void changePattern() override;

    virtual void monitorMidi(uint32_t tick, const MidiMessage &message) override;

    virtual bool activity() const override { return _activity; }
    virtual bool gateOutput(int index) const override { return false; }
    virtual float cvOutput(int index) const override { return 0.f; }

    const HarmonySequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const HarmonySequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _currentStep; }

    void setMonitorStep(int index);

private:
    void beat(uint32_t tick, uint32_t relativeTick, uint32_t divisor);

    HarmonyTrack &_harmonyTrack;

    HarmonySequence *_sequence;
    SequenceState _sequenceState;
    int _currentStep;
    uint32_t _startBeat;

    bool _activity;
};
