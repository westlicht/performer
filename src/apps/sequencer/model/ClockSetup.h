#pragma once

#include "Serialize.h"

#include "core/math/Math.h"

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

    // mode

    Mode mode() const { return _mode; }
    void setMode(Mode mode) {
        if (mode != _mode) {
            _mode = mode;
            _dirty = true;
        }
    }

    // clockInputPPQN

    int clockInputPPQN() const { return _clockInputPPQN; }
    void setClockInputPPQN(int clockInputPPQN) {
        clockInputPPQN = clamp(clockInputPPQN, 1, 48);
        if (clockInputPPQN != _clockInputPPQN) {
            _clockInputPPQN = clockInputPPQN;
            _dirty = true;
        }
    }

    // clockInputMode

    ClockMode clockInputMode() const { return _clockInputMode; }
    void setClockInputMode(ClockMode mode) {
        if (mode != _clockInputMode) {
            _clockInputMode = mode;
            _dirty = true;
        }
    }

    // clockOutputPPQN

    int clockOutputPPQN() const { return _clockOutputPPQN; }
    void setClockOutputPPQN(int clockOutputPPQN) {
        clockOutputPPQN = clamp(clockOutputPPQN, 1, 48);
        if (clockOutputPPQN != _clockOutputPPQN) {
            _clockOutputPPQN = clockOutputPPQN;
            _dirty = true;
        }
    }

    // clockOutputPulse

    int clockOutputPulse() const { return _clockOutputPulse; }
    void setClockOutputPulse(int clockOutputPulse) {
        clockOutputPulse = clamp(clockOutputPulse, 1, 5);
        if (clockOutputPulse != _clockOutputPulse) {
            _clockOutputPulse = clockOutputPulse;
            _dirty = true;
        }
    }

    // clockOutputMode

    ClockMode clockOutputMode() const { return _clockOutputMode; }
    void setClockOutputMode(ClockMode mode) {
        if (mode != _clockOutputMode) {
            _clockOutputMode = mode;
            _dirty = true;
        }
    }

    // midiRx

    bool midiRx() const { return _midiRx; }
    void setMidiRx(bool midiRx) {
        if (midiRx != _midiRx) {
            _midiRx = midiRx;
            _dirty = true;
        }
    }

    // midiTx

    bool midiTx() const { return _midiTx; }
    void setMidiTx(bool midiTx) {
        if (midiTx != _midiTx) {
            _midiTx = midiTx;
            _dirty = true;
        }
    }

    // usbRx

    bool usbRx() const { return _usbRx; }
    void setUsbRx(bool usbRx) {
        if (usbRx != _usbRx) {
            _usbRx = usbRx;
            _dirty = true;
        }
    }

    // usbTx

    bool usbTx() const { return _usbTx; }
    void setUsbTx(bool usbTx) {
        if (usbTx != _usbTx) {
            _usbTx = usbTx;
            _dirty = true;
        }
    }

    // dirty

    bool isDirty() const { return _dirty; }
    void clearDirty() { _dirty = false; }

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
    bool _dirty = true;
};
