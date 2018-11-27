#pragma once

#include "EncoderEvent.h"
#include "MidiEvent.h"
#include "FrameBuffer.h"

#include <functional>

#include <cstdint>

namespace sim {

struct Target {
    std::function<void()> create;
    std::function<void()> destroy;
    std::function<void()> update;
};

struct TargetTickHandler {
    virtual void setTick(uint32_t tick) {}
};

struct TargetInputHandler {
    virtual void writeButton(int index, bool pressed) {}
    virtual void writeEncoder(EncoderEvent event) {}
    virtual void writeAdc(int channel, uint16_t value) {}
    virtual void writeDigitalInput(int pin, bool value) {}
    virtual void writeMidiInput(MidiEvent event) {}
};

struct TargetOutputHandler {
    virtual void writeLed(int index, bool red, bool green) {}
    virtual void writeGateOutput(int channel, bool value) {}
    virtual void writeDac(int channel, uint16_t value) {}
    virtual void writeDigitalOutput(int pin, bool value) {}
    virtual void writeLcd(const FrameBuffer &frameBuffer) {}
    virtual void writeMidiOutput(MidiEvent event) {}
};

} // namespace sim
