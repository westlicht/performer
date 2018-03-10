#pragma once

#include <cstdint>

class TrackSetup;
class Sequence;

class SequenceEngine {
public:
    virtual ~SequenceEngine() {}

    virtual void setup(const TrackSetup &trackSetup) = 0;

    virtual void reset() = 0;
    virtual void tick(uint32_t tick) = 0;

    virtual void setSequence(const Sequence &sequence) = 0;
    virtual void setSwing(int swing) = 0;
    virtual void setMute(bool mute) = 0;
    virtual void setFill(bool fill) = 0;

    virtual bool gate() const = 0;
    virtual bool gateOutput() const = 0;
    virtual float cvOutput() const = 0;
};
