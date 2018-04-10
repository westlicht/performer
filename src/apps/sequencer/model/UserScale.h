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
    void setMode(Mode mode) {
        if (mode != _mode) {
            _mode = mode;
            clearItems();
        }
    }

    void editMode(int value, bool shift) {
        setMode(ModelUtils::adjustedEnum(mode(), value));
    }

    void printMode(StringBuilder &str) const {
        str(modeName(mode()));
    }

    // size

    int size() const { return _size; }
    void setSize(int size) { _size = clamp(size, _mode == Mode::Note ? 1 : 2, CONFIG_USER_SCALE_SIZE); }

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
            noteName(str, index, Scale::Short1);
            break;
        case Mode::Voltage:
            str("%+.3fV", _items[index] * (1.f / 1000.f));
            break;
        case Mode::Last:
            break;
        }
    }

    //----------------------------------------
    // Methods
    //----------------------------------------

    UserScale(const char *name);
    UserScale();

    void clear();
    void clearItems();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    //----------------------------------------
    // Scale implementation
    //----------------------------------------

    bool isChromatic() const override { return mode() == Mode::Note; }

    void noteName(StringBuilder &str, int note, Format format) const override {
        static const char *names[] = { "1", "1#", "2", "2#", "3", "4", "4#", "5", "5#", "6", "6#", "7" };

        switch (_mode) {
        case Mode::Note: {
            int octave = note >= 0 ? (note / _size) : (note - _size + 1) / _size;
            int index = note - octave * _size;
            switch (format) {
            case Short1:
                str(names[index]);
                break;
            case Short2:
                str("%+d", octave);
                break;
            case Long:
                str("%s%+d", names[index], octave);
                break;
            }
            break;
        }
        case Mode::Voltage: {
            float volts = noteVolts(note);
            switch (format) {
            case Short1:
                str("%c", volts < 0.f ? '-' : '+');
                break;
            case Short2:
                str("%.1f", std::abs(volts));
                break;
            case Long:
                str("%+.3fV", volts);
                break;
            }
            break;
        }
        case Mode::Last:
            break;
        }
    }

    float noteVolts(int note) const override {
        int notesPerOctave_ = notesPerOctave();
        int octave = note >= 0 ? (note / notesPerOctave_) : (note - notesPerOctave_ + 1) / notesPerOctave_;
        int index = note - octave * notesPerOctave_;
        switch (_mode) {
        case Mode::Note:
            return octave + _items[index] * (1.f / 12.f);
        case Mode::Voltage:
            return (octave * (_items[_size - 1] - _items[0]) + _items[index]) * (1.f / 1000.f);
        case Mode::Last:
            break;
        }
        return 0.f;
    }

    int notesPerOctave() const override {
        return _mode == Mode::Note ? _size : _size - 1;
    }

    static Array userScales;

private:
    Mode _mode;
    uint8_t _size;
    ItemArray _items;
};
