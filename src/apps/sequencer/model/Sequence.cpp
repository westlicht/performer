#include "Sequence.h"
#include "Model.h"

void Sequence::clear() {
    switch (_trackMode) {
    case Types::TrackMode::Note:    _sequence.noteSequence.clear(); break;
    case Types::TrackMode::Curve:   _sequence.curveSequence.clear(); break;
    case Types::TrackMode::Last:    break;
    }
}

void Sequence::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_trackMode);
    switch (_trackMode) {
    case Types::TrackMode::Note:    _sequence.noteSequence.write(context); break;
    case Types::TrackMode::Curve:   _sequence.curveSequence.write(context); break;
    case Types::TrackMode::Last:    break;
    }
}

void Sequence::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_trackMode);
    switch (_trackMode) {
    case Types::TrackMode::Note:    _sequence.noteSequence.read(context); break;
    case Types::TrackMode::Curve:   _sequence.curveSequence.read(context); break;
    case Types::TrackMode::Last:    break;
    }
}
