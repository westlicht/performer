#pragma once

#include "Config.h"
#include "Serialize.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>

#include <cstdint>

class TimeSignature {
public:
    TimeSignature() = default;

    TimeSignature(int beats, int note) {
        setNote(note);
        setBeats(beats);
    }

    int beats() const { return _beats; }
    void setBeats(int beats) {
        _beats = clamp(beats, 1, _note * 2);
    }

    int note() const { return _note; }
    void setNote(int note) {
        auto it = std::find(validNotes.begin(), validNotes.end(), uint8_t(note));
        if (it == validNotes.end()) {
            _note = DefaultNote;
        } else {
            _note = *it;
        }
        setBeats(beats());
    }

    void edit(int value, bool shift) {
        auto toIndex = [this] () {
            int index = 0;
            for (auto it = validNotes.begin(); *it < note() && it < validNotes.end(); ++it) {
                index += *it * 2;
            }
            return index + beats() - 1;
        };

        auto fromIndex = [this] (int index) {
            auto it = validNotes.begin();
            while (index >= *it * 2 && it < validNotes.end() - 1) {
                index -= *it * 2;
                ++it;
            }
            setNote(*it);
            setBeats(index + 1);
        };

        if (shift) {
            auto it = std::find(validNotes.begin(), validNotes.end(), _note);
            it += value;
            if (it >= validNotes.begin() && it < validNotes.end()) {
                setNote(*it);
            }
        } else {
            fromIndex(toIndex() + value);
        }
    }

    void print(StringBuilder &str) const {
        str("%d/%d", beats(), note());
    }

    uint32_t noteDivisor() const {
        return (CONFIG_PPQN * 4) / note();
    }

    uint32_t measureDivisor() const {
        return beats() * noteDivisor();
    }

    void write(WriteContext &context) const {
        auto &writer = context.writer;
        writer.write(_beats);
        writer.write(_note);
    }

    void read(ReadContext &context) {
        auto &reader = context.reader;
        reader.read(_beats);
        reader.read(_note);
    }

private:
    static const std::array<uint8_t, 6> validNotes;

    static constexpr int DefaultBeats = 4;
    static constexpr int DefaultNote = 4;

    uint8_t _beats = DefaultBeats;
    uint8_t _note = DefaultNote;
};
