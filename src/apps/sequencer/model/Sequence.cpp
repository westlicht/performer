#include "Sequence.h"
#include "Model.h"

void Sequence::clear() {
    switch (_trackMode) {
    case Types::TrackMode::Note:    _sequence.noteSequence.clear(); break;
    case Types::TrackMode::Curve:   _sequence.curveSequence.clear(); break;
    case Types::TrackMode::Last:    break;
    }
}

void Sequence::write(WriteContext &context, int index) const {
    switch (_trackMode) {
    case Types::TrackMode::Note:    _sequence.noteSequence.write(context, index); break;
    case Types::TrackMode::Curve:   _sequence.curveSequence.write(context, index); break;
    case Types::TrackMode::Last:    break;
    }
}

void Sequence::read(ReadContext &context, int index) {
    switch (_trackMode) {
    case Types::TrackMode::Note:    _sequence.noteSequence.read(context, index); break;
    case Types::TrackMode::Curve:   _sequence.curveSequence.read(context, index); break;
    case Types::TrackMode::Last:    break;
    }
}
