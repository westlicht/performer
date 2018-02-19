#pragma once

#include "SortedQueue.h"

#include "model/TrackSetup.h"
#include "model/NoteSequence.h"

class NoteSequenceEngine {
public:
    void setup(const TrackSetup &trackSetup);

    const NoteSequence &sequence() const { return *_sequence; }
    void setSequence(const NoteSequence &sequence) { _sequence = &sequence; }

    void reset();

    void tick(uint32_t tick);

    bool gateOutput() const { return _gateOutput; }
    float cvOutput() const { return _cvOutput; }

    int currentStep() const { return _currentStep; }

private:
    void advance();

    const NoteSequence *_sequence;

    // bool _muted = false;
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
