#pragma once

#include "SequenceEngine.h"
#include "SortedQueue.h"

#include "model/TrackSetup.h"
#include "model/Sequence.h"

class NoteSequenceEngine : public SequenceEngine {
public:
    virtual void setup(const TrackSetup &trackSetup) override;

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;

    virtual void setSequence(const Sequence &sequence) override;
    virtual void setMute(bool mute) override;
    virtual void setFill(bool fill) override;

    virtual bool gate() const override { return _gate; }
    virtual bool gateOutput() const override { return _gateOutput; }
    virtual float cvOutput() const override { return _cvOutput; }

    const NoteSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const NoteSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _currentStep; }

private:
    void advance();

    const NoteSequence *_sequence;

    bool _mute;
    bool _fill;
    bool _gate;
    bool _gateOutput;
    float _cvOutput;
    int _currentStep;
    int8_t _direction;

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
};
