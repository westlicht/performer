#include "TargetStateTracker.h"

namespace sim {

TargetStateTracker::TargetStateTracker(TargetState &targetState) :
    _targetState(targetState)
{}

// TargetInputHandler

void TargetStateTracker::writeButton(int index, bool pressed) {
    _targetState.button.set(index, pressed);
}

void TargetStateTracker::writeAdc(int channel, uint16_t value) {
    _targetState.adc.set(channel, value);
}

void TargetStateTracker::writeDigitalInput(int pin, bool value) {
    _targetState.digitalInput.set(pin, value);
}

// TargetOutputHandler

void TargetStateTracker::writeLed(int index, bool red, bool green) {
    _targetState.led.set(index, red, green);
}

void TargetStateTracker::writeGateOutput(int channel, bool value) {
    _targetState.gateOutput.set(channel, value);
}

void TargetStateTracker::writeDac(int channel, uint16_t value) {
    _targetState.dac.set(channel, value);
}

void TargetStateTracker::writeDigitalOutput(int pin, bool value) {
    _targetState.digitalOutput.set(pin, value);
}

void TargetStateTracker::writeLcd(const FrameBuffer &frameBuffer) {
    _targetState.lcd.set(frameBuffer);
}

} // namespace sim
