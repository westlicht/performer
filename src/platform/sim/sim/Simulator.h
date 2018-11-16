#pragma once

#include "EncoderEvent.h"
#include "MidiEvent.h"
#include "Trace.h"

#include <array>
#include <functional>
#include <string>
#include <vector>

class MidiMessage;

namespace sim {

struct Target {
    std::function<void()> create;
    std::function<void()> destroy;
    std::function<void()> update;
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
    virtual void writeLcd(uint8_t *frameBuffer) {}
    virtual void writeMidiOutput(MidiEvent event) {}
};

class Simulator : public TargetOutputHandler, TargetInputHandler {
public:
    Simulator(Target target);
    ~Simulator();

    void wait(int ms);
    void setButton(int index, bool pressed);
    void setEncoder(bool pressed);
    void rotateEncoder(int direction);
    void setAdc(int channel, float voltage);
    void setDio(int pin, bool state);
    void sendMidi(int port, const MidiMessage &message);

    void screenshot(const std::string &filename);

    const TargetState &targetState() const { return _targetState; }
    const TargetTrace &targetTrace() const { return _targetTrace; }

    double ticks();

    typedef std::function<void()> UpdateCallback;

    void addUpdateCallback(UpdateCallback callback);

    // Target input/output handling

    void registerTargetInputObserver(TargetInputHandler *observer);
    void registerTargetOutputObserver(TargetOutputHandler *observer);

    // TargetInputHandler
    void writeButton(int index, bool pressed) override;
    void writeEncoder(EncoderEvent event) override;
    void writeAdc(int channel, uint16_t value) override;
    void writeDigitalInput(int pin, bool value) override;
    void writeMidiInput(MidiEvent event) override;

    // TargetOutputHandler
    void writeLed(int index, bool red, bool green) override;
    void writeGateOutput(int channel, bool value) override;
    void writeDac(int channel, uint16_t value) override;
    void writeDigitalOutput(int pin, bool value) override;
    void writeLcd(uint8_t *frameBuffer) override;
    void writeMidiOutput(MidiEvent event) override;

    static Simulator &instance();

private:
    void step();

    Target _target;
    bool _targetCreated = false;

    uint32_t _tick = 0;

    std::vector<TargetInputHandler *> _targetInputObservers;
    std::vector<TargetOutputHandler *> _targetOutputObservers;

    std::vector<UpdateCallback> _updateCallbacks;

    TargetState _targetState;
    TargetTrace _targetTrace;

    bool _writeTrace = false;
};

} // namespace sim
