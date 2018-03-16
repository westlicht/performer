#pragma once

#include "SequenceEngine.h"
#include "SequenceState.h"
#include "SortedQueue.h"
#include "Groove.h"

#include "model/TrackSetup.h"
#include "model/Sequence.h"

class NoteSequenceEngine : public SequenceEngine {
public:
    virtual void setup(const TrackSetup &trackSetup) override;

    virtual void setSequence(const Sequence &sequence) override;

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;

    virtual const SequenceState *sequenceState() const override { return &_sequenceState; }

    virtual bool gate() const override { return _gate; }
    virtual bool gateOutput() const override { return _gateOutput; }
    virtual float cvOutput() const override { return _cvOutput; }

    const NoteSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const NoteSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _sequenceState.step(); }

private:
    uint32_t applySwing(uint32_t tick);

    const TrackSetup *_trackSetup;
    const NoteSequence *_sequence;

    SequenceState _sequenceState;

    bool _gate;
    bool _gateOutput;
    float _cvOutput;

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

    struct CV {
        uint32_t tick;
        float cv;
    };

    struct CVCompare {
        bool operator()(const CV &a, const CV &b) {
            return a.tick < b.tick;;
        }
    };

    SortedQueue<CV, 16, CVCompare> _cvQueue;
};
