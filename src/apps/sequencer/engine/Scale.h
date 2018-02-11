#pragma once

#include "core/utils/StringBuilder.h"

#include <algorithm>

#include <cstdint>

class Scale {
public:
    Scale(const char *name) :
        _name(name)
    {}

    const char *name() const { return _name; }

    virtual void shortName(int note, int line, StringBuilder &str) const = 0;
    virtual void longName(int note, StringBuilder &str) const = 0;

    virtual float noteVolts(int note) const = 0;

    virtual int octave() const = 0;

    static const Scale &scale(int index);

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

    void shortName(int note, int line, StringBuilder &str) const override {

    }

    void longName(int note, StringBuilder &str) const override {
        str("%+.1fV", note * _interval);
    }

    float noteVolts(int note) const override {
        return note * _interval;
    }

    int octave() const override {
        return std::max(1, int(1.f / _interval));
    }

private:
    float _interval;
};

class ChromaticScale : public Scale {
public:
    static constexpr uint8_t Flat = 0x40;
    static constexpr uint8_t Sharp = 0x80;

    ChromaticScale(const char *name, const uint8_t *notes, uint8_t noteCount) :
        Scale(name),
        _notes(notes),
        _noteCount(noteCount)
    {}

    void shortName(int note, int line, StringBuilder &str) const override {
        int octave = note >= 0 ? (note / _noteCount) : (note - _noteCount + 1) / _noteCount;
        if (line == 0) {
            str("%+d", octave);
        } else if (line == 1) {
            int index = note - octave * _noteCount;
            uint8_t noteInfo = _notes[index];
            int wholeNote = noteInfo & 0xf;
            const char *intonation = noteInfo & Flat ? "b" : (noteInfo & Sharp ? "#" : "");
            str("%s%d", intonation, wholeNote);
        }
    }

    void longName(int note, StringBuilder &str) const override {
        int octave = note >= 0 ? (note / _noteCount) : (note - _noteCount + 1) / _noteCount;
        int index = note - octave * _noteCount;
        uint8_t noteInfo = _notes[index];
        int wholeNote = noteInfo & 0xf;
        const char *intonation = (noteInfo & Flat) ? "b" : ((noteInfo & Sharp) ? "#" : "");
        str("%d-%s%d-%d", octave, intonation, wholeNote, toSemiNotes(noteInfo));
    }

    float noteVolts(int note) const override {
        int octave = note >= 0 ? (note / _noteCount) : (note - _noteCount + 1) / _noteCount;
        int index = note - octave * _noteCount;
        return octave + toSemiNotes(_notes[index]) * (1.f / 12.f);
    }

    int octave() const override {
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
