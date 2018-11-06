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

    enum class ShiftMode : uint8_t {
        Restart,
        Pause,
        Last
    };

    static const char *shiftModeName(ShiftMode mode) {
        switch (mode) {
        case ShiftMode::Restart:    return "Restart";
        case ShiftMode::Pause:      return "Pause";
        case ShiftMode::Last:       break;
        }
        return nullptr;
    }

    enum class ClockInputMode : uint8_t {
        Reset = 0,
        Run,
        StartStop,
        Last
    };

    static const char *clockInputModeName(ClockInputMode mode) {
        switch (mode) {
        case ClockInputMode::Reset:     return "Reset";
        case ClockInputMode::Run:       return "Run";
        case ClockInputMode::StartStop: return "Start/Stop";
        case ClockInputMode::Last:      break;
        }
        return nullptr;
    }

    enum class ClockOutputMode : uint8_t {
        Reset = 0,
        Run,
        Last
    };

    static const char *clockOutputModeName(ClockOutputMode mode) {
        switch (mode) {
        case ClockOutputMode::Reset:    return "Reset";
        case ClockOutputMode::Run:      return "Run";
        case ClockOutputMode::Last:     break;
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

    // shiftMode

    ShiftMode shiftMode() const { return _shiftMode; }
    void setShiftMode(ShiftMode shiftMode) {
        _shiftMode = ModelUtils::clampedEnum(shiftMode);
    }

    void editShiftMode(int value, int shift) {
        setShiftMode(ModelUtils::adjustedEnum(shiftMode(), value));
    }

    void printShiftMode(StringBuilder &str) const {
        str(shiftModeName(shiftMode()));
    }

    // clockInputDivisor

    int clockInputDivisor() const { return _clockInputDivisor; }
    void setClockInputDivisor(int clockInputDivisor) {
        clockInputDivisor = clamp(clockInputDivisor, 1, 192);
        if (clockInputDivisor != _clockInputDivisor) {
            _clockInputDivisor = clockInputDivisor;
            _dirty = true;
        }
    }

    void editClockInputDivisor(int value, int shift) {
        setClockInputDivisor(ModelUtils::adjustedByDivisor(clockInputDivisor(), value, shift));
    }

    void printClockInputDivisor(StringBuilder &str) const {
        ModelUtils::printDivisor(str, clockInputDivisor());
    }

    // clockInputMode

    ClockInputMode clockInputMode() const { return _clockInputMode; }
    void setClockInputMode(ClockInputMode mode) {
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
        str(clockInputModeName(clockInputMode()));
    }

    // clockOutputDivisor

    int clockOutputDivisor() const { return _clockOutputDivisor; }
    void setClockOutputDivisor(int clockOutputDivisor) {
        clockOutputDivisor = clamp(clockOutputDivisor, 2, 192);
        if (clockOutputDivisor != _clockOutputDivisor) {
            _clockOutputDivisor = clockOutputDivisor;
            _dirty = true;
        }
    }

    void editClockOutputDivisor(int value, int shift) {
        setClockOutputDivisor(ModelUtils::adjustedByDivisor(clockOutputDivisor(), value, shift));
    }

    void printClockOutputDivisor(StringBuilder &str) const {
        ModelUtils::printDivisor(str, clockOutputDivisor());
    }

    // clockOutputSwing

    bool clockOutputSwing() const { return _clockOutputSwing; }
    void setClockOutputSwing(bool clockOutputSwing) {
        if (clockOutputSwing != _clockOutputSwing) {
            _clockOutputSwing = clockOutputSwing;
            _dirty = true;
        }
    }

    void editClockOutputSwing(int value, int shift) {
        setClockOutputSwing(value > 0);
    }

    void printClockOutputSwing(StringBuilder &str) const {
        ModelUtils::printYesNo(str, _clockOutputSwing);
    }

    // clockOutputPulse

    int clockOutputPulse() const { return _clockOutputPulse; }
    void setClockOutputPulse(int clockOutputPulse) {
        clockOutputPulse = clamp(clockOutputPulse, 1, 20);
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

    ClockOutputMode clockOutputMode() const { return _clockOutputMode; }
    void setClockOutputMode(ClockOutputMode mode) {
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
        str(clockOutputModeName(clockOutputMode()));
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
    ShiftMode _shiftMode;
    uint8_t _clockInputDivisor;
    ClockInputMode _clockInputMode;
    uint8_t _clockOutputDivisor;
    bool _clockOutputSwing;
    uint8_t _clockOutputPulse;
    ClockOutputMode _clockOutputMode;
    bool _midiRx;
    bool _midiTx;
    bool _usbRx;
    bool _usbTx;
    bool _dirty;
};
