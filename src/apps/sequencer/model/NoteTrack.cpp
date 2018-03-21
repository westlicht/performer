#include "NoteTrack.h"

void NoteTrack::clear() {
    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void NoteTrack::write(WriteContext &context) const {
    writeArray(context, _sequences);
}

void NoteTrack::read(ReadContext &context) {
    readArray(context, _sequences);
}
