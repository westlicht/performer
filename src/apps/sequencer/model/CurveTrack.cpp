#include "CurveTrack.h"

void CurveTrack::clear() {
    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void CurveTrack::write(WriteContext &context) const {
    writeArray(context, _sequences);
}

void CurveTrack::read(ReadContext &context) {
    readArray(context, _sequences);
}
