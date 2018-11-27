#include "TargetTraceRecorder.h"

namespace sim {

TargetTraceRecorder::TargetTraceRecorder(TargetTrace &targetTrace) :
    TargetStateTracker(_targetState),
    _targetTrace(targetTrace)
{}

// TargetTickHandler

void TargetTraceRecorder::setTick(uint32_t tick) {
    _tick = tick;
}

// TargetInputHandler

void TargetTraceRecorder::writeButton(int index, bool pressed) {
    TargetStateTracker::writeButton(index, pressed);
    _targetTrace.button.write(_tick, _targetState.button);
}

void TargetTraceRecorder::writeEncoder(EncoderEvent event) {
    _targetTrace.encoder.write(_tick, event);
}

void TargetTraceRecorder::writeAdc(int channel, uint16_t value) {
    TargetStateTracker::writeAdc(channel, value);
    _targetTrace.adc.write(_tick, _targetState.adc);
}

void TargetTraceRecorder::writeDigitalInput(int pin, bool value) {
    TargetStateTracker::writeDigitalInput(pin, value);
    _targetTrace.digitalInput.write(_tick, _targetState.digitalInput);
}

void TargetTraceRecorder::writeMidiInput(MidiEvent event) {
    _targetTrace.midiInput.write(_tick, event);
}

// TargetOutputHandler

void TargetTraceRecorder::writeLed(int index, bool red, bool green) {
    TargetStateTracker::writeLed(index, red, green);
    _targetTrace.led.write(_tick, _targetState.led);
}

void TargetTraceRecorder::writeGateOutput(int channel, bool value) {
    TargetStateTracker::writeGateOutput(channel, value);
    _targetTrace.gateOutput.write(_tick, _targetState.gateOutput);
}

void TargetTraceRecorder::writeDac(int channel, uint16_t value) {
    TargetStateTracker::writeDac(channel, value);
    _targetTrace.dac.write(_tick, _targetState.dac);
}

void TargetTraceRecorder::writeDigitalOutput(int pin, bool value) {
    TargetStateTracker::writeDigitalOutput(pin, value);
    _targetTrace.digitalOutput.write(_tick, _targetState.digitalOutput);
}

void TargetTraceRecorder::writeLcd(const FrameBuffer &frameBuffer) {
    TargetStateTracker::writeLcd(frameBuffer);
    _targetTrace.lcd.write(_tick, _targetState.lcd);
}

void TargetTraceRecorder::writeMidiOutput(MidiEvent event) {
    _targetTrace.midiOutput.write(_tick, event);
}

} // namespace sim
