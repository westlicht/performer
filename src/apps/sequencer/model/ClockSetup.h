#pragma once

#include "Serialize.h"
#include "ModelUtils.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <cstdint>

class ClockSetup {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

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

    //----------------------------------------
    // Properties
    //----------------------------------------

    // mode

    Mode mode() const { return _mode; }
    void setMode(Mode mode) {
        mode = ModelUtils::clampedEnum(mode);
        if (mode != _mode) {
            _mode = mode;
            _dirty = true;
        }
    }

    void editMode(int value, int shift) {
        setMode(ModelUtils::adjustedEnum(mode(), value));
    }

    void printMode(StringBuilder &str) const {
        str(modeName(mode()));
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

    void editClockInputPPQN(int value, int shift) {
        setClockInputPPQN(ModelUtils::adjustedByPowerOfTwo(clockInputPPQN(), value, shift));
    }

    void printClockInputPPQN(StringBuilder &str) const {
        str("%d", clockInputPPQN());
    }

    // clockInputMode

    ClockMode clockInputMode() const { return _clockInputMode; }
    void setClockInputMode(ClockMode mode) {
        mode = ModelUtils::clampedEnum(mode);
        if (mode != _clockInputMode) {
            _clockInputMode = mode;
            _dirty = true;
        }
    }

    void editClockInputMode(int value, int shift) {
        setClockInputMode(ModelUtils::adjustedEnum(clockInputMode(), value));
    }

    void printClockInputMode(StringBuilder &str) const {
        str(clockModeName(clockInputMode()));
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

    void editClockOutputPPQN(int value, int shift) {
        setClockOutputPPQN(ModelUtils::adjustedByPowerOfTwo(clockOutputPPQN(), value, shift));
    }

    void printClockOutputPPQN(StringBuilder &str) const {
        str("%d", clockOutputPPQN());
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

    void editClockOutputPulse(int value, int shift) {
        setClockOutputPulse(clockOutputPulse() + value);
    }

    void printClockOutputPulse(StringBuilder &str) const {
        str("%dms", clockOutputPulse());
    }

    // clockOutputMode

    ClockMode clockOutputMode() const { return _clockOutputMode; }
    void setClockOutputMode(ClockMode mode) {
        mode = ModelUtils::clampedEnum(mode);
        if (mode != _clockOutputMode) {
            _clockOutputMode = mode;
            _dirty = true;
        }
    }

    void editClockOutputMode(int value, int shift) {
        setClockOutputMode(ModelUtils::adjustedEnum(clockOutputMode(), value));
    }

    void printClockOutputMode(StringBuilder &str) const {
        str(clockModeName(clockOutputMode()));
    }

    // midiRx

    bool midiRx() const { return _midiRx; }
    void setMidiRx(bool midiRx) {
        if (midiRx != _midiRx) {
            _midiRx = midiRx;
            _dirty = true;
        }
    }

    void editMidiRx(int value, bool shift) {
        setMidiRx(value > 0);
    }

    void printMidiRx(StringBuilder &str) const {
        ModelUtils::printYesNo(str, midiRx());
    }

    // midiTx

    bool midiTx() const { return _midiTx; }
    void setMidiTx(bool midiTx) {
        if (midiTx != _midiTx) {
            _midiTx = midiTx;
            _dirty = true;
        }
    }

    void editMidiTx(int value, bool shift) {
        setMidiTx(value > 0);
    }

    void printMidiTx(StringBuilder &str) const {
        ModelUtils::printYesNo(str, midiTx());
    }

    // usbRx

    bool usbRx() const { return _usbRx; }
    void setUsbRx(bool usbRx) {
        if (usbRx != _usbRx) {
            _usbRx = usbRx;
            _dirty = true;
        }
    }

    void editUsbRx(int value, bool shift) {
        setUsbRx(value > 0);
    }

    void printUsbRx(StringBuilder &str) const {
        ModelUtils::printYesNo(str, usbRx());
    }

    // usbTx

    bool usbTx() const { return _usbTx; }
    void setUsbTx(bool usbTx) {
        if (usbTx != _usbTx) {
            _usbTx = usbTx;
            _dirty = true;
        }
    }

    void editUsbTx(int value, bool shift) {
        setUsbTx(value > 0);
    }

    void printUsbTx(StringBuilder &str) const {
        ModelUtils::printYesNo(str, usbTx());
    }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    bool isDirty() const { return _dirty; }
    void clearDirty() { _dirty = false; }

private:
    Mode _mode;
    uint8_t _clockInputPPQN;
    ClockMode _clockInputMode;
    uint8_t _clockOutputPPQN;
    uint8_t _clockOutputPulse;
    ClockMode _clockOutputMode;
    bool _midiRx;
    bool _midiTx;
    bool _usbRx;
    bool _usbTx;
    bool _dirty;
};
