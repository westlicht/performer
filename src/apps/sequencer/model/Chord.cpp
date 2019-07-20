#include "Chord.h"

struct QualityInfo {
    const char *name;
    uint8_t noteCount;
    uint8_t notes[5];
};

static const QualityInfo qualityInfos[] = {
    { "Maj", 3, { 0, 4, 7 } },
    { "Min", 3, { 0, 3, 7 } },
    { "Aug", 3, { 0, 4, 8 } },
    { "Dim", 3, { 0, 4, 6 } },
    { "Maj 6th", 4, { 0, 4, 7, 9 } },
    { "Min 6th", 4, { 0, 3, 7, 9 } },
    { "Maj 7th", 4, { 0, 4, 7, 11 } },
    { "Min 7th", 4, { 0, 3, 7, 10 } },
};

const char *Chord::qualityName(Quality quality) {
    int index = int(quality);
    return (index >= 0 && index < int(Quality::Last)) ? qualityInfos[index].name : nullptr;
}

const char *Chord::voicingName(Voicing voicing) {
    switch (voicing) {
    case Voicing::Root:     return "Root";
    case Voicing::First:    return "First";
    case Voicing::Second:   return "Second";
    case Voicing::Third:    return "Third";
    case Voicing::Last:
        break;
    }
    return nullptr;
}

void Chord::chordNotes(int root, Quality quality, Voicing voicing, uint8_t &noteCount, uint8_t *notes) {
    int index = int(quality);
    if (index >= 0 && index < int(Quality::Last)) {
        const auto &info = qualityInfos[index];
        noteCount = info.noteCount;
        for (int i = 0; i < noteCount; ++i) {
            notes[i] = root + info.notes[i];
        }
    }

    switch (voicing) {
    case Voicing::Third:
        notes[2] += 12;
        // fallthrough
    case Voicing::Second:
        notes[1] += 12;
        // fallthrough
    case Voicing::First:
        notes[0] += 12;
        break;
    case Voicing::Root:
    case Voicing::Last:
        break;
    }
}
