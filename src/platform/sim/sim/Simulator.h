#pragma once

#include "Common.h"
#include "Window.h"
#include "Audio.h"
#include "Instrument.h"
#include "MIDI.h"

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

    double ticks();

    typedef std::function<void()> UpdateCallback;

    void addUpdateCallback(UpdateCallback callback);

    // Hardware IO emulation
    void writeGate(int channel, bool value);
    void writeDAC(int channel, uint16_t value);
    uint16_t readADC(int channel);

    // MIDI emulation
    enum MIDIPort {
        MIDIHardwarePort,
        MIDIUSBHostPort,
    };

    typedef std::function<void(uint8_t)> MIDIRecvCallback;

    void sendMIDI(int port, uint8_t data);
    void sendMIDI(int port, const uint8_t *data, size_t length);
    void recvMIDI(int port, MIDIRecvCallback callback);

    static Simulator &instance();

private:
    void setupInstruments();

    Window _window;
    Audio _audio;
    std::vector<Instrument::Ptr> _instruments;

    double _timerFrequency;
    double _timerStart;

    std::array<bool, 8> _gate;
    std::array<uint16_t, 8> _dac;

    std::vector<UpdateCallback> _updateCallbacks;

    static const std::vector<std::string> _midiPortName;
    MIDI _midi;
};

} // namespace sim
