#pragma once

#include "TrackEngine.h"
#include "SequenceState.h"
#include "SortedQueue.h"
#include "Groove.h"
#include "RecordHistory.h"

class StageTrackEngine : public TrackEngine {
public:
    StageTrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(engine, model, track, linkedTrackEngine),
        _stageTrack(track.stageTrack())
    {
        reset();
    }

    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::Stage; }

    virtual void reset() override;
    virtual void restart() override;
    virtual void tick(uint32_t tick) override;
    virtual void update(float dt) override;

    virtual void changePattern() override;

    virtual void monitorMidi(uint32_t tick, const MidiMessage &message) override;

    virtual bool activity() const override { return _activity; }
    virtual bool gateOutput(int index) const override { return _channels[index % 2].gateOutput; }
    virtual float cvOutput(int index) const override { return _channels[index % 2].cvOutput; }

    const StageSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const StageSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _currentStep; }

    void setMonitorStep(int index);

private:
    void beat(uint32_t tick, uint32_t relativeTick, uint32_t divisor);
    void triggerBeat(uint32_t beat, uint32_t tick, uint32_t divisor);
    void triggerStep(uint32_t tick, uint32_t divisor);

    StageTrack &_stageTrack;

    StageSequence *_sequence;
    SequenceState _sequenceState;
    int _currentStep;
    uint32_t _startBeat;

    int _monitorStepIndex = -1;
    bool _monitorOverrideActive = false;

    bool _activity;

    struct Gate {
        uint32_t tick;
        bool gate;
    };

    struct GateCompare {
        bool operator()(const Gate &a, const Gate &b) {
            return a.tick < b.tick;;
        }
    };

    struct Cv {
        uint32_t tick;
        float cv;
        bool slide;
    };

    struct CvCompare {
        bool operator()(const Cv &a, const Cv &b) {
            return a.tick < b.tick;
        }
    };

    struct Channel {
        bool gateOutput;
        float cvOutput;
        float cvOutputTarget;
        bool slideActive;
        SortedQueue<Gate, 16, GateCompare> gateQueue;
        SortedQueue<Cv, 16, CvCompare> cvQueue;
    };

    std::array<Channel, 2> _channels;
};
