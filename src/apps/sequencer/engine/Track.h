#pragma once

#include "SortedQueue.h"

#include "model/Model.h"


class TrackEngine {
public:
    TrackEngine();

    void setSequence(Sequence &sequence) { _sequence = &sequence; }

    const Sequence &sequence() const { return *_sequence; }
          Sequence &sequence()       { return *_sequence; }

    bool muted() const { return _muted; }
    void setMuted(bool muted) { _muted = muted; }
    void toggleMuted() { setMuted(!muted()); }

    bool gate() const { return _gate; }
    bool gateOutput() const { return _gateOutput; }

    float cv() const { return _cv; }

    int currentStep() const { return _currentStep; }

    void reset();

    void tick(uint32_t tick);

private:
    void advance(const Sequence &sequence);

    Sequence *_sequence = nullptr;

    bool _muted = false;
    bool _gate;         // gate of sequence
    bool _gateOutput;   // gate at output (with mutes)
    float _cv;
    int _currentStep;
    int8_t _direction;

    typedef std::pair<uint32_t, bool> Gate;
    struct GateCompare {
        bool operator()(const Gate &a, const Gate &b) {
            return a.first < b.first;;
        }
    };
    SortedQueue<Gate, 16, GateCompare> _gateQueue;
};
