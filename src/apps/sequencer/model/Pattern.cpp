#include "Pattern.h"

void Pattern::clear() {
    for (auto &sequence : _sequences) {
        // TODO
        sequence.clear(TrackSetup::Mode::Note);
    }
}

void Pattern::write(WriteContext &context, int index) const {
    writeArray(context, _sequences);
}

void Pattern::read(ReadContext &context, int index) {
    readArray(context, _sequences);
}
