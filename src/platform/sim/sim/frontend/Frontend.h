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

#include "sim/Simulator.h"

#include <string>
#include <vector>

#include <cstdint>

namespace sim {

class Frontend : private TargetOutputHandler {
public:
    Frontend(Simulator &simulator);

    void run();

    void close();

private:
    bool terminate() const;
    void step();
    void update();
    void render();
    void delay(int ms);

    double ticks() const;

    void setupWindow();
    void setupEncoder();
    void setupLcd();
    void setupButtonLedMatrix();
    void setupAdc();
    void setupDio();

    void setupMidi();
    void setupInstruments();

    // TargetOutputHandler
    void writeLed(int index, bool red, bool green) override;
    void writeGateOutput(int channel, bool value) override;
    void writeDac(int channel, uint16_t value) override;
    void writeDigitalOutput(int pin, bool value) override;
    void writeLcd(uint8_t *frameBuffer) override;
    void writeMidiOutput(MidiEvent event) override;

    sdl::Init _sdl;
    Simulator &_simulator;
    Window _window;
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

    Display::Ptr _display;
    std::vector<Button::Ptr> _buttons;
    std::vector<Led::Ptr> _leds;
    std::vector<Label::Ptr> _labels;
    Led::Ptr _clockOutputLed;
    Led::Ptr _resetOutputLed;
};

} // namespace sim
