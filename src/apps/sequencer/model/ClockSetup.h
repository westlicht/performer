#pragma once

#include "Serialize.h"

#include <cstdint>

class ClockSetup {
public:
    enum class Mode : uint8_t {
        Auto = 0,
        Master,
        Slave,
        Last
    };

    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::Auto:    return "Auto";
        case Mode::Master:  return "Master";
        case Mode::Slave:   return "Slave";
        case Mode::Last:    break;
        }
        return nullptr;
    }

    enum class ClockMode : uint8_t {
        Reset = 0,
        StartStop,
        Last
    };

    static const char *clockModeName(ClockMode mode) {
        switch (mode) {
        case ClockMode::Reset:       return "Reset";
        case ClockMode::StartStop:   return "Start/Stop";
        case ClockMode::Last:        break;
        }
        return nullptr;
    }

    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

    uint8_t clockInputPPQN() const { return _clockInputPPQN; }
    void setClockInputPPQN(uint8_t clockInputPPQN) { _clockInputPPQN = clockInputPPQN; }

    ClockMode clockInputMode() const { return _clockInputMode; }
    void setClockInputMode(ClockMode mode) { _clockInputMode = mode; }

    uint8_t clockOutputPPQN() const { return _clockOutputPPQN; }
    void setClockOutputPPQN(uint8_t clockOutputPPQN) { _clockOutputPPQN = clockOutputPPQN; }

    uint8_t clockOutputPulse() const { return _clockOutputPulse; }
    void setClockOutputPulse(uint8_t clockOutputPulse) { _clockOutputPulse = clockOutputPulse; }

    ClockMode clockOutputMode() const { return _clockOutputMode; }
    void setClockOutputMode(ClockMode mode) { _clockOutputMode = mode; }

    bool midiRx() const { return _midiRx; }
    void setMidiRx(bool midiRx) { _midiRx = midiRx; }

    bool midiTx() const { return _midiTx; }
    void setMidiTx(bool midiTx) { _midiTx = midiTx; }

    bool usbRx() const { return _usbRx; }
    void setUsbRx(bool usbRx) { _usbRx = usbRx; }

    bool usbTx() const { return _usbTx; }
    void setUsbTx(bool usbTx) { _usbTx = usbTx; }

    // Serialization

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    Mode _mode = Mode::Auto;
    uint8_t _clockInputPPQN = 16;
    ClockMode _clockInputMode = ClockMode::Reset;
    uint8_t _clockOutputPPQN = 16;
    uint8_t _clockOutputPulse = 1;
    ClockMode _clockOutputMode = ClockMode::Reset;
    bool _midiRx = true;
    bool _midiTx = true;
    bool _usbRx = false;
    bool _usbTx = false;
};
