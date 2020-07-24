#pragma once

#include "Target.h"
#include "TargetStateTracker.h"
#include "TargetTrace.h"

#include <array>
#include <functional>
#include <string>
#include <vector>

class MidiMessage;

namespace sim {

class Simulator : public TargetInputHandler, public TargetOutputHandler {
public:
    Simulator(Target target);
    virtual ~Simulator();

    void wait(int ms);
    void setButton(int index, bool pressed);
    void setEncoder(bool pressed);
    void rotateEncoder(int direction);
    void setAdc(int channel, float voltage);
    void setDio(int pin, bool state);
    void sendMidi(int port, const MidiMessage &message);

    void screenshot(const std::string &filename);

    const TargetState &targetState() const { return _targetState; }

    double ticks();

    typedef std::function<void()> UpdateCallback;

    void addUpdateCallback(UpdateCallback callback);

    // Target input/output handling

    void registerTargetTickObserver(TargetTickHandler *observer);
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
    void writeLcd(const FrameBuffer &frameBuffer) override;
    void writeMidiOutput(MidiEvent event) override;

    static Simulator &instance();

private:
    void step();

    Target _target;
    bool _targetCreated = false;

    uint32_t _tick = 0;

    std::vector<TargetTickHandler *> _targetTickObservers;
    std::vector<TargetInputHandler *> _targetInputObservers;
    std::vector<TargetOutputHandler *> _targetOutputObservers;

    std::vector<UpdateCallback> _updateCallbacks;

    TargetState _targetState;
    TargetStateTracker _targetStateTracker;
};

} // namespace sim
