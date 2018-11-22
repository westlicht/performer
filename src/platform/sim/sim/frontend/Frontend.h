#pragma once

#include "Common.h"
#include "Window.h"
#include "Audio.h"
#include "InstrumentSetup.h"
#include "Midi.h"
#include "ClockSource.h"

#include "widgets/Button.h"
#include "widgets/Display.h"
#include "widgets/Encoder.h"
#include "widgets/Label.h"
#include "widgets/Led.h"
#include "widgets/Rotary.h"
#include "widgets/Jack.h"

#include "sim/Simulator.h"

#include <string>
#include <vector>

#include <cstdint>

namespace sim {

class Frontend : private TargetInputHandler, TargetOutputHandler {
public:
    Frontend(Simulator &simulator);

    void run();
    void step();

    void close();

private:
    bool terminate() const;
    void update();
    void render();
    void delay(int ms);

    double ticks() const;

    void setupWindow();
    void setupFrontpanel();
    void setupControls();

    void setupMidi();
    void setupInstruments();

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

    sdl::Init _sdl;
    Simulator &_simulator;
    Audio _audio;
    std::unique_ptr<InstrumentSetup> _instruments;

    double _timerFrequency;
    double _timerStart;

    uint32_t _lastUpdateTicks = 0;
    double _lastRenderTicks = 0.0;

    Midi _midi;
    std::shared_ptr<Midi::Port> _midiPort;
    std::shared_ptr<Midi::Port> _usbMidiPort;

    std::unique_ptr<ClockSource> _clockSource;

    Window::Ptr _window;
    Encoder::Ptr _encoder;
    Display::Ptr _lcd;
    Jack::Ptr _midiInputJack;
    Jack::Ptr _midiOutputJack;
    std::vector<Jack::Ptr> _digitalInputJacks;
    std::vector<Jack::Ptr> _digitalOutputJacks;
    std::vector<Jack::Ptr> _cvInputJacks;
    std::vector<Jack::Ptr> _gateOutputJacks;
    std::vector<Jack::Ptr> _cvOutputJacks;
    std::vector<Button::Ptr> _buttons;
    std::vector<Led::Ptr> _leds;
    std::vector<Label::Ptr> _labels;
};

} // namespace sim
