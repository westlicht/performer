#include "Scale.h"
#include "UserScale.h"

#define ARRAY_SIZE(_array_) (sizeof(_array_) / sizeof(_array_[0]))
#define NOTE_SCALE(_name_, _title_, _chromatic_, ...) \
static const uint16_t _name_##_notes[] = { __VA_ARGS__ }; \
static const NoteScale _name_(_title_, _chromatic_, ARRAY_SIZE(_name_##_notes), _name_##_notes);

NOTE_SCALE(semitoneScale, "Semitones", true, 0, 128, 256, 384, 512, 640, 768, 896, 1024, 1152, 1280, 1408)

NOTE_SCALE(majorScale, "Major", true, 0, 256, 512, 640, 896, 1152, 1408)
NOTE_SCALE(minorScale, "Minor", true, 0, 256, 384, 640, 896, 1024, 1280)

NOTE_SCALE(majorBluesScale, "Major Blues", true, 0, 384, 512, 896, 1152, 1280)
NOTE_SCALE(minorBluesScale, "Minor Blues", true, 0, 384, 640, 768, 896, 1280)

NOTE_SCALE(majorPentatonicScale, "Major Pent.", true, 0, 256, 512, 896, 1152)
NOTE_SCALE(minorPentatonicScale, "Minor Pent.", true, 0, 384, 640, 896, 1280)

NOTE_SCALE(folkScale, "Folk", true, 0, 128, 384, 512, 640, 896, 1024, 1280)
NOTE_SCALE(japaneseScale, "Japanese", true, 0, 128, 640, 896, 1024)
NOTE_SCALE(gamelanScale, "Gamelan", true, 0, 128, 384, 896, 1024)
NOTE_SCALE(gypsyScale, "Gypsy", true, 0, 256, 384, 768, 896, 1024, 1408)
NOTE_SCALE(arabianScale, "Arabian", true, 0, 128, 512, 640, 896, 1024, 1408)
NOTE_SCALE(flamencoScale, "Flamenco", true, 0, 128, 512, 640, 896, 1024, 1280)
NOTE_SCALE(wholeToneScale, "Whole Tone", true, 0, 256, 512, 768, 1024, 1280)

// python: [int(round(x * (12 * 128) / float(N))) for x in range(N)]
NOTE_SCALE(tet5Scale, "5-tet", false, 0, 307, 614, 922, 1229);
NOTE_SCALE(tet7Scale, "7-tet", false, 0, 219, 439, 658, 878, 1097, 1317);
NOTE_SCALE(tet19Scale, "19-tet", false, 0, 81, 162, 243, 323, 404, 485, 566, 647, 728, 808, 889, 970, 1051, 1132, 1213, 1293, 1374, 1455);
NOTE_SCALE(tet22Scale, "22-tet", false, 0, 70, 140, 209, 279, 349, 419, 489, 559, 628, 698, 768, 838, 908, 977, 1047, 1117, 1187, 1257, 1327, 1396, 1466);
NOTE_SCALE(tet24Scale, "24-tet", false, 0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960, 1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472);

#undef ARRAY_SIZE
#undef NOTE_SCALE

static const VoltScale voltageScale("Voltage", 0.1f);

static const Scale *scales[] = {
    &semitoneScale,

    &majorScale,
    &minorScale,

    &majorBluesScale,
    &minorBluesScale,

    &majorPentatonicScale,
    &minorPentatonicScale,

    &folkScale,
    &japaneseScale,
    &gamelanScale,
    &gypsyScale,
    &arabianScale,
    &flamencoScale,
    &wholeToneScale,

    &tet5Scale,
    &tet7Scale,
    &tet19Scale,
    &tet22Scale,
    &tet24Scale,

    &voltageScale
};

static const int BuiltinCount = sizeof(scales) / sizeof(Scale *);
static const int UserCount = UserScale::userScales.size();

int Scale::Count = BuiltinCount + UserCount;

const Scale &Scale::get(int index) {
    if (index < BuiltinCount) {
        return *scales[index];
    } else {
        return UserScale::userScales[index - BuiltinCount];
    }
}

const char *Scale::name(int index) {
    if (index < BuiltinCount) {
        return get(index).displayName();
    } else {
        switch (index - BuiltinCount) {
        case 0: return "User1";
        case 1: return "User2";
        case 2: return "User3";
        case 3: return "User4";
        }
    }
    return nullptr;
}
