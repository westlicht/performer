#include "Scale.h"

#define S ChromaticScale::Sharp
#define F ChromaticScale::Flat

static const uint8_t chromaticNotes[] = { 1, S|1, 2, S|2, 3, 4, S|4, 5, S|5, 6, S|6, 7 };
static const uint8_t majorNotes[] = { 1, 2, 3, 4, 5, 6, 7 };
static const uint8_t minorNotes[] = { 1, 2, F|3, 4, 5, 6, 7 };

static const uint8_t algerianNotes[] = { 1, 2, F|3, S|4, 5, F|6, 7 };

// pentatonic
static const uint8_t majorPentatonicNotes[] = { 1, 2, 3, 5, 6 };
static const uint8_t minorPentatonicNotes[] = { 1, F|3, 4, 5, F|7 };

// hexatonic
static const uint8_t wholeToneNotes[] = { 1, 2, 3, S|4, S|5, S|6 };
static const uint8_t augumentedNotes[] = { 1, F|3, 3, 5, S|5, 7 };
static const uint8_t prometheusNotes[] = { 1, 2, 3, S|4, 6, F|7 };
static const uint8_t bluesNotes[] = { 1, F|3, 4, F|5, 5, F|7 };
static const uint8_t tritoneNotes[] = { 1, F|2, 3, F|5, 5, F|7 };


#undef S
#undef F

static const ChromaticScale chromaticScale("Chromatic", chromaticNotes, sizeof(chromaticNotes));
static const ChromaticScale majorScale("Major", majorNotes, sizeof(majorNotes));
static const ChromaticScale minorScale("Minor", minorNotes, sizeof(minorNotes));

static const ChromaticScale algerianScale("Algerian", algerianNotes, sizeof(algerianNotes));

// pentatonic
static const ChromaticScale majorPentatonicScale("Major Pent.", majorPentatonicNotes, sizeof(majorPentatonicNotes));
static const ChromaticScale minorPentatonicScale("Minor Pent.", minorPentatonicNotes, sizeof(minorPentatonicNotes));

// hexatonic
static const ChromaticScale wholeToneScale("Whole Tone", wholeToneNotes, sizeof(wholeToneNotes));
static const ChromaticScale augumentedScale("Augumented", augumentedNotes, sizeof(augumentedNotes));
static const ChromaticScale prometheusScale("Prometheus", prometheusNotes, sizeof(prometheusNotes));
static const ChromaticScale bluesScale("Blues", bluesNotes, sizeof(bluesNotes));
static const ChromaticScale tritoneScale("Tritone", tritoneNotes, sizeof(tritoneNotes));

// x-tet
static const VoltScale tet5Scale("5-tet", 1.f / 5.f);
static const VoltScale tet7Scale("7-tet", 1.f / 7.f);
static const VoltScale tet12Scale("12-tet", 1.f / 12.f);
static const VoltScale tet24Scale("24-tet", 1.f / 24.f);

static const VoltScale Scale1V("1V", 0.1f);

static const Scale *scales[] = {
    &chromaticScale,
    &majorScale,
    &minorScale,
    &algerianScale,
    // pentatonic
    &majorPentatonicScale,
    &minorPentatonicScale,
    // hexatonic
    &wholeToneScale,
    &augumentedScale,
    &prometheusScale,
    &bluesScale,
    &tritoneScale,
    // x-tet
    &tet5Scale,
    &tet7Scale,
    &tet12Scale,
    &tet24Scale,
    // voltage
    &Scale1V
};

int Scale::Count = sizeof(scales) / sizeof(Scale *);

const Scale &Scale::get(int index) {
    return *scales[index];
}
