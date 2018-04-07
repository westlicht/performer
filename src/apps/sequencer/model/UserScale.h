#pragma once

#include "Config.h"

#include "Scale.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "Types.h"

#include "core/math/Math.h"

#include <array>

#include <cstdint>

class UserScale : public Scale {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef std::array<UserScale, CONFIG_USER_SCALE_COUNT> Array;
    typedef std::array<int16_t, CONFIG_USER_SCALE_SIZE> ItemArray;

    enum class Mode : uint8_t {
        Note,
        Voltage,
        Last,
    };

    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::Note:    return "Note";
        case Mode::Voltage: return "Voltage";
        default:            break;
        }
        return nullptr;
    }

    //----------------------------------------
    // Properties
    //----------------------------------------

    // mode

    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

    void editMode(int value, bool shift) {
        setMode(ModelUtils::adjustedEnum(mode(), value));
    }

    void printMode(StringBuilder &str) const {
        str(modeName(mode()));
    }

    // size

    int size() const { return _size; }
    void setSize(int size) { _size = clamp(size, 1, CONFIG_USER_SCALE_SIZE); }

    void editSize(int value, bool shift) {
        setSize(size() + value);
    }

    void printSize(StringBuilder &str) const {
        str("%d", size());
    }

    // items

    const ItemArray &items() const { return _items; }
          ItemArray &items()       { return _items; }

    int item(int index) const { return _items[index]; }
    void setItem(int index, int value) {
        switch (_mode) {
        case Mode::Note:
            _items[index] = clamp(value, 0, 11);
            break;
        case Mode::Voltage:
            _items[index] = clamp(value, -5000, 5000);
            break;
        case Mode::Last:
            break;
        }
    }

    void editItem(int index, int value, int shift) {
        switch (_mode) {
        case Mode::Note:
            setItem(index, item(index) + value);
            break;
        case Mode::Voltage:
            setItem(index, item(index) + value * (shift ? 100 : 1));
            break;
        case Mode::Last:
            break;
        }
    }

    void printItem(int index, StringBuilder &str) const {
        switch (_mode) {
        case Mode::Note:
            Types::printNote(str, item(index));
            break;
        case Mode::Voltage:
            str("%dmV", item(index));
            break;
        case Mode::Last:
            break;
        }
    }

    //----------------------------------------
    // Methods
    //----------------------------------------

    UserScale();

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    //----------------------------------------
    // Scale implementation
    //----------------------------------------

    bool isChromatic() const override { return mode() == Mode::Note; }

    void shortName(int note, int line, StringBuilder &str) const override {
    }

    void longName(int note, StringBuilder &str) const override {
    }

    float noteVolts(int note) const override {
        return 0.f;
    }

    int octave() const override {
        return 1;
    }

    static Array userScales;

private:
    Mode _mode;
    uint8_t _size;
    ItemArray _items;
};
