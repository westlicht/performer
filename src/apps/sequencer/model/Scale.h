#pragma once

#include "core/utils/StringBuilder.h"
#include "core/math/Math.h"

#include <algorithm>

#include <cstdint>
#include <cmath>

class Scale {
public:
    enum Format {
        Short1,
        Short2,
        Long,
    };

    Scale(const char *name) :
        _name(name)
    {}

    const char *name() const { return _name; }

    virtual bool isChromatic() const = 0;

    virtual void noteName(StringBuilder &str, int note, Format format = Long) const = 0;
    virtual float noteVolts(int note) const = 0;
    virtual int noteFromVolts(float volts) const = 0;

    virtual int notesPerOctave() const = 0;

    static int Count;
    static const Scale &get(int index);

private:
    const char *_name;
};

class VoltScale : public Scale {
public:
    VoltScale(const char *name, float interval) :
        Scale(name),
        _interval(interval)
    {
    }

    bool isChromatic() const override { return false; }

    void noteName(StringBuilder &str, int note, Format format) const override {
        switch (format) {
        case Short1:
            str("%c", note < 0 ? '-' : '+');
            break;
        case Short2:
            str("%.1f", std::abs(note * _interval));
            break;
        case Long:
            str("%+.3fV", note * _interval);
            break;
        }
    }

    float noteVolts(int note) const override {
        return note * _interval;
    }

    int noteFromVolts(float volts) const override {
        return int(std::floor(volts / _interval));
    }

    int notesPerOctave() const override {
        return std::max(1, int(1.f / _interval));
    }

private:
    float _interval;
};

class ChromaticScale : public Scale {
public:
    static constexpr uint8_t Flat = 0x40;
    static constexpr uint8_t Sharp = 0x80;

    bool isChromatic() const override { return true; }

    ChromaticScale(const char *name, const uint8_t *notes, uint8_t noteCount) :
        Scale(name),
        _notes(notes),
        _noteCount(noteCount)
    {}

    void noteName(StringBuilder &str, int note, Format format) const override {
        int octave = note >= 0 ? (note / _noteCount) : (note - _noteCount + 1) / _noteCount;
        int index = note - octave * _noteCount;
        uint8_t noteInfo = _notes[index];
        int wholeNote = noteInfo & 0xf;
        const char *intonation = (noteInfo & Flat) ? "b" : ((noteInfo & Sharp) ? "#" : "");

        switch (format) {
        case Short1:
            str("%s%d", intonation, wholeNote);
            break;
        case Short2:
            str("%+d", octave);
            break;
        case Long:
            str("%s%d%+d", intonation, wholeNote, octave);
            break;
        }
    }

    float noteVolts(int note) const override {
        int octave = roundDownDivide(note, _noteCount);
        int index = note - octave * _noteCount;
        return octave + toSemiNotes(_notes[index]) * (1.f / 12.f);
    }

    int noteFromVolts(float volts) const override {
        int semiNotes = std::floor(volts * 12.f + 0.01f);
        int octave = roundDownDivide(semiNotes, 12);
        semiNotes -= octave * 12;

        int index = -1;
        for (int i = 0; i < _noteCount; ++i) {
            int noteSemiNotes = toSemiNotes(_notes[i]);
            if (semiNotes < noteSemiNotes) {
                break;
            }
            index = i;
        }

        if (index == -1) {
            index = _noteCount -1;
            --octave;
        }

        return octave * _noteCount + index;
    }

    int notesPerOctave() const override {
        return _noteCount;
    }

private:
    int toSemiNotes(uint8_t note) const {
        static uint8_t noteToSemiNotes[] = { 0, 2, 4, 5, 7, 9, 11 };
        int semiNotes = noteToSemiNotes[(note & 0xf) - 1];
        semiNotes -= (note & Flat) ? 1 : 0;
        semiNotes += (note & Sharp) ? 1 : 0;
        return semiNotes;
    }

    const uint8_t *_notes;
    uint8_t _noteCount;
};
