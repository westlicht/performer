#include "StageTrack.h"

void StageTrack::writeRouted(Routing::Target target, int intValue, float floatValue) {
    switch (target) {
    case Routing::Target::SlideTime:
        setSlideTime(intValue, true);
        break;
    case Routing::Target::Octave:
        setOctave(intValue, true);
        break;
    case Routing::Target::Transpose:
        setTranspose(intValue, true);
        break;
    case Routing::Target::Rotate:
        setRotate(intValue, true);
        break;
    default:
        break;
    }
}

void StageTrack::clear() {
    setSlideTime(50);
    setOctave(0);
    setTranspose(0);
    setRotate(0);

    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void StageTrack::gateOutputName(int index, StringBuilder &str) const {
    str("Gate%d", (index % 2) + 1);
}

void StageTrack::cvOutputName(int index, StringBuilder &str) const {
    str("CV%d", (index % 2) + 1);
}

void StageTrack::write(VersionedSerializedWriter &writer) const {
    writer.write(_slideTime.base);
    writer.write(_octave.base);
    writer.write(_transpose.base);
    writer.write(_rotate.base);
    writeArray(writer, _sequences);
}

void StageTrack::read(VersionedSerializedReader &reader) {
    reader.read(_slideTime.base);
    reader.read(_octave.base);
    reader.read(_transpose.base);
    reader.read(_rotate.base);
    readArray(reader, _sequences);
}
