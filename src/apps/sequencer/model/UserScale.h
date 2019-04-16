#pragma once

#include "Config.h"

#include "Scale.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "Types.h"
#include "FileDefs.h"

#include "core/math/Math.h"
#include "core/utils/StringUtils.h"

#include <array>

#include <cstdint>

class UserScale : public Scale {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    static constexpr size_t NameLength = FileHeader::NameLength;

    typedef std::array<UserScale, CONFIG_USER_SCALE_COUNT> Array;
    typedef std::array<int16_t, CONFIG_USER_SCALE_SIZE> ItemArray;

    enum class Mode : uint8_t {
        Chromatic,
        Voltage,
        Last,
    };

    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::Chromatic:   return "Chromatic";
        case Mode::Voltage:     return "Voltage";
        default:                break;
        }
        return nullptr;
    }

    //----------------------------------------
    // Properties
    //----------------------------------------

    // name

    const char *name() const { return _name; }
    void setName(const char *name) {
        StringUtils::copy(_name, name, sizeof(_name));
    }

    // mode

    Mode mode() const { return _mode; }
    void setMode(Mode mode) {
        mode = ModelUtils::clampedEnum(mode);
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
    void setSize(int size) {
        _size = clamp(size, _mode == Mode::Chromatic ? 1 : 2, CONFIG_USER_SCALE_SIZE);
    }

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
        case Mode::Chromatic:
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
        case Mode::Chromatic:
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
        case Mode::Chromatic:
            noteNameChromaticMode(str, index, Scale::Short1);
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

    UserScale();

    void clear();
    void clearItems();

    void write(WriteContext &context) const;
    bool read(ReadContext &context);

    fs::Error write(const char *path) const;
    fs::Error read(const char *path);

    //----------------------------------------
    // Scale implementation
    //----------------------------------------

    bool isChromatic() const override {
        return mode() == Mode::Chromatic;
    }

    void noteName(StringBuilder &str, int note, Format format) const override {
        switch (_mode) {
        case Mode::Chromatic:
            noteNameChromaticMode(str, note, format);
            break;
        case Mode::Voltage:
            noteNameVoltageMode(str, note, format);
            break;
        case Mode::Last:
            break;
        }
    }

    float noteToVolts(int note) const override {
        int notesPerOctave_ = notesPerOctave();
        int octave = roundDownDivide(note, notesPerOctave_);
        int index = note - octave * notesPerOctave_;
        switch (_mode) {
        case Mode::Chromatic:
            return octave + _items[index] * (1.f / 12.f);
        case Mode::Voltage:
            return octave * octaveRangeVolts() + _items[index] * (1.f / 1000.f);
        case Mode::Last:
            break;
        }
        return 0.f;
    }

    int noteFromVolts(float volts) const override {
        switch (_mode) {
        case Mode::Chromatic:
            return noteFromVoltsChromaticMode(volts);
        case Mode::Voltage:
            return noteFromVoltsVoltageMode(volts);
        case Mode::Last:
            break;
        }
        return 0;
    }

    int notesPerOctave() const override {
        return _mode == Mode::Chromatic ? _size : _size - 1;
    }

    static Array userScales;

private:
    void noteNameChromaticMode(StringBuilder &str, int note, Format format) const {
        static const char *names[] = { "1", "1#", "2", "2#", "3", "4", "4#", "5", "5#", "6", "6#", "7" };

        int octave = roundDownDivide(note, _size);
        int index = note - octave * _size;
        switch (format) {
        case Short1:
            str(names[_items[index]]);
            break;
        case Short2:
            str("%+d", octave);
            break;
        case Long:
            str("%s%+d", names[index], octave);
            break;
        }
    }

    void noteNameVoltageMode(StringBuilder &str, int note, Format format) const {
        float volts = noteToVolts(note);
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
    }

    int noteFromVoltsChromaticMode(float volts) const {
        int semiNotes = std::floor(volts * 12.f + 0.01f);
        int octave = roundDownDivide(semiNotes, 12);
        semiNotes -= octave * 12;

        int index = -1;
        for (int i = 0; i < _size; ++i) {
            if (semiNotes < _items[i]) {
                break;
            }
            index = i;
        }

        if (index == -1) {
            index = _size -1;
            --octave;
        }

        return octave * _size + index;
    }

    int noteFromVoltsVoltageMode(float volts) const {
        float octaveRange = octaveRangeVolts();
        int octave = int(std::floor(volts / octaveRange));
        volts -= octave * octaveRange;
        int itemValue = int(std::floor(volts * 1000.f));

        int index = -1;
        for (int i = 0; i < _size; ++i) {
            if (itemValue < _items[i]) {
                break;
            }
            index = i;
        }

        if (index == -1) {
            index = _size -1;
            --octave;
        }

        return octave * (_size - 1) + index;
    }

    float octaveRangeVolts() const {
        return (_items[_size - 1] - _items[0]) * (1.f / 1000.f);
    }

    char _name[NameLength + 1];
    Mode _mode;
    uint8_t _size;
    ItemArray _items;
};
