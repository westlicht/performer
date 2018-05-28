#pragma once

#include "TrackEngine.h"
#include "SequenceState.h"
#include "SortedQueue.h"
#include "Groove.h"

#include "model/Track.h"

class NoteTrackEngine : public TrackEngine {
public:
    NoteTrackEngine(const Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(track, linkedTrackEngine),
        _noteTrack(track.noteTrack())
    {}

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;
    virtual void update(float dt) override;
    virtual void changePattern() override;

    virtual const TrackLinkData *linkData() const override { return &_linkData; }

    virtual bool activity() const override { return _gate; }
    virtual bool gateOutput(int index) const override { return _gateOutput; }
    virtual float cvOutput(int index) const override { return _cvOutput; }

    virtual bool idleOutput() const override { return _idleOutput; }
    virtual bool idleGateOutput(int index) const override { return _idleGateOutput; }
    virtual float idleCvOutput(int index) const override { return _idleCvOutput; }
    virtual void clearIdleOutput() override { _idleOutput = false; }

    const NoteSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const NoteSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _currentStep; }

    void setIdleStep(int index);
    void setIdleGate(bool gate);

    static constexpr Track::TrackMode trackMode = Track::TrackMode::Note;

private:
    void triggerStep(uint32_t tick, uint32_t divisor);
    uint32_t applySwing(uint32_t tick);

    const NoteTrack &_noteTrack;

    TrackLinkData _linkData;

    const NoteSequence *_sequence;
    const NoteSequence *_fillSequence;
    SequenceState _sequenceState;
    int _currentStep;

    bool _gate;
    bool _gateOutput;
    float _cvOutput;
    float _cvOutputTarget;
    bool _slideActive;

    bool _idleOutput;
    bool _idleGateOutput;
    float _idleCvOutput;

    struct Gate {
        uint32_t tick;
        bool gate;
    };

    struct GateCompare {
        bool operator()(const Gate &a, const Gate &b) {
            return a.tick < b.tick;;
        }
    };

    SortedQueue<Gate, 16, GateCompare> _gateQueue;

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

    SortedQueue<Cv, 16, CvCompare> _cvQueue;
};
