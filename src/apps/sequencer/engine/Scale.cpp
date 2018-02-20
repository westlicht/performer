#include "Scale.h"

#define S ChromaticScale::Sharp
#define F ChromaticScale::Flat

static const uint8_t chromaticNotes[] = { 1, S|1, 2, S|2, 3, 4, S|4, 5, S|5, 6, S|6, 7 };
static const uint8_t majorNotes[] = { 1, 2, 3, 4, 5, 6, 7 };
static const uint8_t minorNotes[] = { 1, 2, F|3, 4, 5, 6, 7 };
static const uint8_t bluesNotes[] = { 1, F|3, 4, F|5, 5, F|7 };

#undef S
#undef F

static const ChromaticScale chromaticScale("Chromatic", chromaticNotes, sizeof(chromaticNotes));
static const ChromaticScale majorScale("Major", majorNotes, sizeof(majorNotes));
static const ChromaticScale minorScale("Minor", minorNotes, sizeof(minorNotes));
static const ChromaticScale bluesScale("Blues", bluesNotes, sizeof(bluesNotes));

static const VoltScale Scale1V("1V", 0.1f);

static const Scale *scales[] = {
    &chromaticScale,
    &majorScale,
    &minorScale,
    &bluesScale,
    &Scale1V
};

int Scale::Count = sizeof(scales) / sizeof(Scale *);

const Scale &Scale::get(int index) {
    return *scales[index];
}
