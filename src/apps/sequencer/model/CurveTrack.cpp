#include "CurveTrack.h"
#include "ProjectVersion.h"

void CurveTrack::writeRouted(Routing::Target target, int intValue, float floatValue) {
    switch (target) {
    case Routing::Target::SlideTime:
        setSlideTime(intValue, true);
        break;
    case Routing::Target::Rotate:
        setRotate(intValue, true);
        break;
    case Routing::Target::ShapeProbabilityBias:
        setShapeProbabilityBias(intValue, true);
        break;
    case Routing::Target::GateProbabilityBias:
        setGateProbabilityBias(intValue, true);
        break;
    default:
        break;
    }
}

void CurveTrack::clear() {
    setPlayMode(Types::PlayMode::Aligned);
    setFillMode(FillMode::None);
    setSlideTime(0);
    setRotate(0);
    setShapeProbabilityBias(0);
    setGateProbabilityBias(0);

    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void CurveTrack::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_playMode);
    writer.write(_fillMode);
    writer.write(_slideTime.base);
    writer.write(_rotate.base);
    writer.write(_shapeProbabilityBias.base);
    writer.write(_gateProbabilityBias.base);
    writeArray(context, _sequences);
}

void CurveTrack::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_playMode);
    reader.read(_fillMode);
    reader.read(_slideTime.base, ProjectVersion::Version8);
    reader.read(_rotate.base);
    reader.read(_shapeProbabilityBias.base, ProjectVersion::Version15);
    reader.read(_gateProbabilityBias.base, ProjectVersion::Version15);
    readArray(context, _sequences);
}
