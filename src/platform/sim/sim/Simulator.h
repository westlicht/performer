#pragma once

#include "Common.h"
#include "Window.h"
#include "Audio.h"
#include "InstrumentSetup.h"
#include "Midi.h"

#include "widgets/Button.h"

#include <string>

namespace sim {

class Simulator {
public:
    Simulator();

    Window &window() { return _window; }
    Audio &audio() { return _audio; }

    void close();

    bool terminate() const;

    void update();
    void render();
    void delay(int ms);

    double ticks();

    typedef std::function<void()> UpdateCallback;

    void addUpdateCallback(UpdateCallback callback);

    typedef std::function<void(const std::string &filename)> ScreenshotCallback;

    void setScreenshotCallback(ScreenshotCallback callback);
    void screenshot(const std::string &filename = "");

    // Hardware IO emulation
    void writeGate(int channel, bool value);
    void writeDac(int channel, uint16_t value);

    // MIDI emulation
    enum MidiPort {
        MidiHardwarePort,
        MidiUsbHostPort,
    };

    typedef std::function<void(uint8_t)> MidiRecvCallback;

    void sendMidi(int port, uint8_t data);
    void sendMidi(int port, const uint8_t *data, size_t length);
    void recvMidi(int port, MidiRecvCallback callback);

    static Simulator &instance();

private:
    void setupInstruments();

    Window _window;
    Audio _audio;
    std::unique_ptr<InstrumentSetup> _instruments;

    double _timerFrequency;
    double _timerStart;

    double _lastRenderTicks = 0.0;

    std::array<bool, 8> _gate;
    std::array<uint16_t, 8> _dac;

    std::vector<UpdateCallback> _updateCallbacks;

    ScreenshotCallback _screenshotCallback;
    std::shared_ptr<Button> _screenshotButton;

    static const std::vector<std::string> _midiPortName;
    Midi _midi;
};

} // namespace sim
