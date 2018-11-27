#pragma once

#include "Target.h"
#include "TargetState.h"

namespace sim {

class TargetStateTracker : public TargetInputHandler, public TargetOutputHandler {
public:
    TargetStateTracker(TargetState &targetState);

    TargetState &targetState() { return _targetState; }

    // TargetInputHandler
    virtual void writeButton(int index, bool pressed) override;
    virtual void writeAdc(int channel, uint16_t value) override;
    virtual void writeDigitalInput(int pin, bool value) override;

    // TargetOutputHandler
    virtual void writeLed(int index, bool red, bool green) override;
    virtual void writeGateOutput(int channel, bool value) override;
    virtual void writeDac(int channel, uint16_t value) override;
    virtual void writeDigitalOutput(int pin, bool value) override;
    virtual void writeLcd(const FrameBuffer &frameBuffer) override;

private:
    TargetState &_targetState;
};

} // namespace sim
