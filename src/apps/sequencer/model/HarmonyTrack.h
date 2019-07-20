#pragma once

#include "Config.h"
#include "Types.h"
#include "HarmonySequence.h"
#include "Serialize.h"

class HarmonyTrack {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef std::array<HarmonySequence, CONFIG_PATTERN_COUNT + CONFIG_SNAPSHOT_COUNT> HarmonySequenceArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // sequences

    const HarmonySequenceArray &sequences() const { return _sequences; }
          HarmonySequenceArray &sequences()       { return _sequences; }

    const HarmonySequence &sequence(int index) const { return _sequences[index]; }
          HarmonySequence &sequence(int index)       { return _sequences[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    HarmonyTrack() { clear(); }

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    void setTrackIndex(int trackIndex) {
        _trackIndex = trackIndex;
        for (auto &sequence : _sequences) {
            sequence.setTrackIndex(trackIndex);
        }
    }

    int8_t _trackIndex = -1;

    HarmonySequenceArray _sequences;

    friend class Track;
};
