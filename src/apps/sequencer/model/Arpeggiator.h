#pragma once

#include "Serialize.h"
#include "ModelUtils.h"

#include "core/utils/StringBuilder.h"

#include <cstdint>

class Arpeggiator {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    enum class Mode : uint8_t {
        PlayOrder,
        Up,
        Down,
        UpDown,
        DownUp,
        UpAndDown,
        DownAndUp,
        Converge,
        Diverge,
        Random,
        Last
    };

    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::PlayOrder:   return "Play Order";
        case Mode::Up:          return "Up";
        case Mode::Down:        return "Down";
        case Mode::UpDown:      return "Up Down";
        case Mode::DownUp:      return "Down Up";
        case Mode::UpAndDown:   return "Up & Down";
        case Mode::DownAndUp:   return "Down & Up";
        case Mode::Converge:    return "Converge";
        case Mode::Diverge:     return "Diverge";
        case Mode::Random:      return "Random";
        case Mode::Last:        break;
        }
        return nullptr;
    }

    static uint8_t modeSerialize(Mode mode) {
        switch (mode) {
        case Mode::PlayOrder:   return 0;
        case Mode::Up:          return 1;
        case Mode::Down:        return 2;
        case Mode::UpDown:      return 3;
        case Mode::DownUp:      return 4;
        case Mode::UpAndDown:   return 5;
        case Mode::DownAndUp:   return 6;
        case Mode::Converge:    return 7;
        case Mode::Diverge:     return 8;
        case Mode::Random:      return 9;
        case Mode::Last:        break;
        }
        return 0;
    }

    //----------------------------------------
    // Properties
    //----------------------------------------

    // enabled

    bool enabled() const { return _enabled; }
    void setEnabled(bool enabled) {
        _enabled = enabled;
    }

    void editEnabled(int value, bool shift) {
        setEnabled(value > 0);
    }

    void printEnabled(StringBuilder &str) const {
        ModelUtils::printYesNo(str, enabled());
    }

    // hold

    bool hold() const { return _hold; }
    void setHold(bool hold) {
        _hold = hold;
    }

    void editHold(int value, bool shift) {
        setHold(value > 0);
    }

    void printHold(StringBuilder &str) const {
        ModelUtils::printYesNo(str, hold());
    }

    // mode

    Mode mode() const { return _mode; }
    void setMode(Mode mode) {
        _mode = ModelUtils::clampedEnum(mode);
    }

    void editMode(int value, bool shift) {
        setMode(ModelUtils::adjustedEnum(mode(), value));
    }

    void printMode(StringBuilder &str) const {
        str(modeName(mode()));
    }

    // divisor

    int divisor() const { return _divisor; }
    void setDivisor(int divisor) {
        _divisor = ModelUtils::clampDivisor(divisor);
    }

    void editDivisor(int value, bool shift) {
        setDivisor(ModelUtils::adjustedByDivisor(divisor(), value, shift));
    }

    void printDivisor(StringBuilder &str) const {
        ModelUtils::printDivisor(str, divisor());
    }

    // gateLength

    int gateLength() const { return _gateLength; }
    void setGateLength(int gateLength) {
        _gateLength = clamp(gateLength, 1, 100);
    }

    void editGateLength(int value, bool shift) {
        setGateLength(gateLength() + value * (shift ? 10 : 1));
    }

    void printGateLength(StringBuilder &str) const {
        str("%d%%", gateLength());
    }

    // octaves

    int octaves() const { return _octaves; }
    void setOctaves(int octaves) {
        _octaves = clamp(octaves, -10, 10);
    }

    void editOctaves(int value, bool shift) {
        setOctaves(octaves() + value);
    }

    void printOctaves(StringBuilder &str) const {
        int value = octaves();
        if (value > 5) {
            str("Up Down %d", value - 5);
        } else if (value > 0) {
            str("Up %d", value);
        } else if (value == 0) {
            str("Off");
        } else if (value >= -5) {
            str("Down %d", -value);
        } else if (value >= -10) {
            str("Down Up %d", -(value + 5));
        }
    }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    bool _enabled;
    bool _hold;
    Mode _mode;
    uint16_t _divisor;
    uint8_t _gateLength;
    int8_t _octaves;
};
