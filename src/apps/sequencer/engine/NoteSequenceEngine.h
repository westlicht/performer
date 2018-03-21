#pragma once

#include "SequenceEngine.h"
#include "SequenceState.h"
#include "SortedQueue.h"
#include "Groove.h"

#include "model/Track.h"
#include "model/Sequence.h"

class NoteSequenceEngine : public SequenceEngine {
public:
    virtual void setup(const Track &track) override;

    virtual void setSequence(const Sequence &sequence) override;

    virtual const SequenceLinkData *sequenceLinkData() const override { return &_sequenceLinkData; }

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;

    virtual bool gate() const override { return _gate; }
    virtual bool gateOutput() const override { return _gateOutput; }
    virtual float cvOutput() const override { return _cvOutput; }

    const NoteSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const NoteSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _sequenceState.step(); }

private:
    void triggerStep(uint32_t tick, uint32_t divisor);
    uint32_t applySwing(uint32_t tick);

    const Track *_track;
    const NoteSequence *_sequence;

    SequenceState _sequenceState;
    SequenceLinkData _sequenceLinkData;

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
