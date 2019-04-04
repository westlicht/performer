#include "CurveTrack.h"

void CurveTrack::writeRouted(Routing::Target target, int intValue, float floatValue) {
    switch (target) {
    case Routing::Target::Rotate:
        setRotate(intValue, true);
        break;
    default:
        break;
    }
}

void CurveTrack::clear() {
    setPlayMode(Types::PlayMode::Aligned);
    setFillMode(Types::FillMode::None);
    setRotate(0);

    _routed.clear();

    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void CurveTrack::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_playMode);
    writer.write(_fillMode);
    writer.write(_rotate.base);
    writeArray(context, _sequences);
}

void CurveTrack::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_playMode);
    reader.read(_fillMode);
    reader.read(_rotate.base);
    readArray(context, _sequences);
}
