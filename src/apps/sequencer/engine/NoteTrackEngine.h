#pragma once

#include "TrackEngine.h"
#include "SequenceState.h"
#include "SortedQueue.h"
#include "Groove.h"

class NoteTrackEngine : public TrackEngine {
public:
    NoteTrackEngine(const Model &model, const Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(model, track, linkedTrackEngine),
        _noteTrack(track.noteTrack())
    {
        reset();
    }

    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::Note; }

    virtual void reset() override;
    virtual void setRunning(bool running) override { _running = running; }
    virtual void tick(uint32_t tick) override;
    virtual void update(float dt) override;
    virtual void receiveMidi(MidiPort port, int channel, const MidiMessage &message) override;
    virtual void changePattern() override;

    virtual const TrackLinkData *linkData() const override { return &_linkData; }

    virtual void setSelected(bool selected) override { _selected = selected; }

    virtual bool activity() const override { return _activity; }
    virtual bool gateOutput(int index) const override { return _gateOutput; }
    virtual float cvOutput(int index) const override { return _cvOutput; }

    const NoteSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const NoteSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _currentStep; }

    void setMonitorStep(int index);

private:
    void triggerStep(uint32_t tick, uint32_t divisor);
    uint32_t applySwing(uint32_t tick);

    const NoteTrack &_noteTrack;

    TrackLinkData _linkData;

    const NoteSequence *_sequence;
    const NoteSequence *_fillSequence;
    SequenceState _sequenceState;
    int _currentStep;

    int _monitorStepIndex = -1;

    bool _midiGate = false;
    uint8_t _midiNote;

    bool _running = false;
    bool _selected = false;

    bool _activity;
    bool _gateOutput;
    float _cvOutput;
    float _cvOutputTarget;
    bool _slideActive;

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
