#include "HarmonyTrack.h"

void HarmonyTrack::clear() {
    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void HarmonyTrack::write(WriteContext &context) const {
    writeArray(context, _sequences);
}

void HarmonyTrack::read(ReadContext &context) {
    readArray(context, _sequences);
}
