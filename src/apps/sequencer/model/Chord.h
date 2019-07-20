#pragma once

#include <cstdint>

class Chord {
public:
    enum class Quality : uint8_t {
        Maj,
        Min,
        Aug,
        Dim,
        Maj6,
        Min6,
        Maj7,
        Min7,
        Last
    };

    enum class Voicing : uint8_t {
        Root,
        First,
        Second,
        Third,
        Last,
    };

    static const char *qualityName(Quality quality);
    static const char *voicingName(Voicing voicing);

    static void chordNotes(int root, Quality quality, Voicing voicing, uint8_t &noteCount, uint8_t *notes);
};
