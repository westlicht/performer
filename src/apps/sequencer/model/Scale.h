#pragma once

#include "Types.h"
#include "Chord.h"

#include "core/utils/StringBuilder.h"
#include "core/math/Math.h"

#include <algorithm>

#include <cstdint>
#include <cmath>

class ChordScale;

class Scale {
public:
    enum Format {
        Short1,
        Short2,
        Long,
    };

    Scale(const char *name) :
        _displayName(name)
    {}

    virtual bool isChromatic() const = 0;

    virtual void noteName(StringBuilder &str, int note, int rootNote, Format format = Long) const = 0;
    virtual float noteToVolts(int note) const = 0;
    virtual int noteFromVolts(float volts) const = 0;

    virtual int notesPerOctave() const = 0;

    static int Count;
    static const Scale &get(int index);
    static const char *name(int index);

    static ChordScale &chordScale();

private:
    const char *displayName() const { return _displayName; }

    const char *_displayName;
};


class NoteScale : public Scale {
public:
    NoteScale(const char *name, bool chromatic, uint16_t noteCount, const uint16_t *notes) :
        Scale(name),
        _chromatic(chromatic),
        _noteCount(noteCount),
        _notes(notes)
    {
    }

    bool isChromatic() const override {
        return _chromatic;
    }

    void noteName(StringBuilder &str, int note, int rootNote, Format format) const override {
        bool printNote = format == Short1 || format == Long;
        bool printOctave = format == Short2 || format == Long;

        int octave = roundDownDivide(note, _noteCount);

        int noteIndex = 0;
        if (isChromatic()) {
            noteIndex = _notes[note - octave * _noteCount] / 128 + rootNote;
            while (noteIndex >= 12) {
                noteIndex -= 12;
                octave += 1;
            }
        } else {
            noteIndex = note - octave * _noteCount + 1;
        }

        if (printNote) {
            if (isChromatic()) {
                Types::printNote(str, noteIndex);
            } else {
                str("%d", noteIndex);
            }
        }

        if (printOctave) {
            str("%+d", octave);
        }
    }

    float noteToVolts(int note) const override {
        int octave = roundDownDivide(note, _noteCount);
        int index = note - octave * _noteCount;
        return octave + _notes[index] * (1.f / 1536.f);
    }

    int noteFromVolts(float volts) const override {
        volts += 0.01f;
        int octave = std::floor(volts);
        float fractional = volts - octave;

        int index = -1;
        for (int i = 0; i < _noteCount; ++i) {
            if (fractional < _notes[i] * (1.f / 1536.f)) {
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
    bool _chromatic;
    uint16_t _noteCount;
    const uint16_t *_notes;
};

class VoltScale : public Scale {
public:
    VoltScale(const char *name, float interval) :
        Scale(name),
        _interval(interval)
    {
    }

    bool isChromatic() const override {
        return false;
    }

    void noteName(StringBuilder &str, int note, int rootNote, Format format) const override {
        switch (format) {
        case Short1:
            str("%.1f", std::abs(note * _interval));
            break;
        case Short2:
            str("%c", note < 0 ? '-' : '+');
            break;
        case Long:
            str("%+.2fV", note * _interval);
            break;
        }
    }

    float noteToVolts(int note) const override {
        return note * _interval;
    }

    int noteFromVolts(float volts) const override {
        return int(std::floor(volts / _interval));
    }

    int notesPerOctave() const override {
        return std::max(1, int(std::round(1.f / _interval)));
    }

private:
    float _interval;
};

class ChordScale : public Scale {
public:
    ChordScale(const char *name) :
        Scale(name)
    {
        updateChord();
    }

    void setRoot(int root) {
        _root = root;
        updateChord();
    }

    void setQuality(Chord::Quality quality) {
        _quality = quality;
        updateChord();
    }

    void setVoicing(Chord::Voicing voicing) {
        _voicing = voicing;
        updateChord();
    }

    bool isChromatic() const override {
        return true;
    }

    void noteName(StringBuilder &str, int note, Format format) const override {
        int octave = roundDownDivide(note, _noteCount);
        int index = note - octave * _noteCount + 1;
        switch (format) {
        case Short1:
            str("%d", index);
            break;
        case Short2:
            str("%+d", octave);
            break;
        case Long:
            str("%d%+d", index, octave);
            break;
        }
    }

    float noteToVolts(int note) const override {
        int octave = roundDownDivide(note, _noteCount);
        int index = note - octave * _noteCount;
        return octave + _notes[index] * (1.f / 12.f);
    }

    int noteFromVolts(float volts) const override {
        volts += 0.01f;
        int octave = std::floor(volts);
        float fractional = volts - octave;

        int index = -1;
        for (int i = 0; i < _noteCount; ++i) {
            if (fractional < _notes[i] * (1.f / 12.f)) {
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
    void updateChord() {
        Chord::chordNotes(_root, _quality, _voicing, _noteCount, _notes);
    }

    int16_t _root = 0;
    Chord::Quality _quality = Chord::Quality(0);
    Chord::Voicing _voicing = Chord::Voicing(0);

    uint8_t _noteCount;
    uint8_t _notes[5];
};

