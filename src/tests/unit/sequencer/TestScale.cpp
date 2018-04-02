#include "UnitTest.h"

#include "core/utils/StringBuilder.h"

#include "apps/sequencer/engine/Scale.cpp"

#include <array>
#include <cstdint>

UNIT_TEST("Scale") {

    CASE("scales") {
        for (int i = 0; i < Scale::Count; ++i) {
            const auto &scale = Scale::get(i);
            int octave = scale.octave();

            DBG("----------------------------------------");
            DBG("%s", scale.name());
            DBG("octave has %d notes", octave);
            DBG("----------------------------------------");
            DBG("%-8s %-8s %-8s %-8s %-8s", "note", "volts", "short1", "short2", "long");

            for (int note = -octave; note <= octave; ++note) {
                FixedStringBuilder<8> shortName1;
                FixedStringBuilder<8> shortName2;
                FixedStringBuilder<16> longName;
                scale.shortName(note, 0, shortName1);
                scale.shortName(note, 1, shortName2);
                scale.longName(note, longName);
                float volts = scale.noteVolts(note);
                DBG("%-8d %-8.3f %-8s %-8s %-8s", note, volts, (const char *)(shortName1), (const char *)(shortName2), (const char *)(longName));
                // DBG("note = %d, volts = %f", note, volts);
            }

        }

    }

}
