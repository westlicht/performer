#include "Sequence.h"
#include "Model.h"

void Sequence::clear(TrackSetup::Mode mode) {
    switch (mode) {
    case TrackSetup::Mode::Note:    _sequence.noteSequence.clear(); break;
    case TrackSetup::Mode::Curve:   _sequence.curveSequence.clear(); break;
    case TrackSetup::Mode::Last:    break;
    }
}

void Sequence::write(WriteContext &context, int index) const {
    switch (context.project.trackSetup(index).mode()) {
    case TrackSetup::Mode::Note:    _sequence.noteSequence.write(context, index); break;
    case TrackSetup::Mode::Curve:   _sequence.curveSequence.write(context, index); break;
    case TrackSetup::Mode::Last:    break;
    }
}

void Sequence::read(ReadContext &context, int index) {
    switch (context.project.trackSetup(index).mode()) {
    case TrackSetup::Mode::Note:    _sequence.noteSequence.read(context, index); break;
    case TrackSetup::Mode::Curve:   _sequence.curveSequence.read(context, index); break;
    case TrackSetup::Mode::Last:    break;
    }
}
