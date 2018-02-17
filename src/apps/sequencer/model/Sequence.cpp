#include "Sequence.h"
#include "Model.h"
#include "TrackSetup.h"

void Sequence::write(WriteContext &context, int index) const {
    auto &writer = context.writer;
    writer.write(_playMode);
    writer.write(_firstStep);
    writer.write(_lastStep);

    switch (context.project.trackSetup(index).mode()) {
    case TrackSetup::Mode::Note:    _sequence.noteSequence.write(context, index); break;
    case TrackSetup::Mode::Curve:   _sequence.curveSequence.write(context, index); break;
    case TrackSetup::Mode::Last:    break;
    }
}

void Sequence::read(ReadContext &context, int index) {
    auto &reader = context.reader;
    reader.read(_playMode);
    reader.read(_firstStep);
    reader.read(_lastStep);

    switch (context.project.trackSetup(index).mode()) {
    case TrackSetup::Mode::Note:    _sequence.noteSequence.read(context, index); break;
    case TrackSetup::Mode::Curve:   _sequence.curveSequence.read(context, index); break;
    case TrackSetup::Mode::Last:    break;
    }
}
