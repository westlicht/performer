#pragma once

#include <cstdint>

class TrackSetup;
class Sequence;
class SequenceState;

struct SequenceLinkData {
    uint32_t divisor;
    uint32_t relativeTick;
    SequenceState *sequenceState;
};

class SequenceEngine {
public:
    virtual ~SequenceEngine() {}

    virtual void setup(const TrackSetup &trackSetup) = 0;

    virtual void setSequence(const Sequence &sequence) = 0;

    virtual void setLinkedSequenceEngine(const SequenceEngine *linkedSequenceEngine) {
        _linkedSequenceEngine = linkedSequenceEngine;
    }

    virtual void setSwing(int swing) {
        _swing = swing;
    }

    virtual void setMute(bool mute) {
        _mute = mute;
    }

    virtual void setFill(bool fill) {
        _fill = fill;
    }

    virtual const SequenceLinkData *sequenceLinkData() const { return nullptr; }

    virtual void reset() = 0;
    virtual void tick(uint32_t tick) = 0;

    virtual bool gate() const = 0;
    virtual bool gateOutput() const = 0;
    virtual float cvOutput() const = 0;

protected:
    const SequenceEngine *_linkedSequenceEngine = nullptr;
    uint8_t _swing;
    bool _mute;
    bool _fill;
};
