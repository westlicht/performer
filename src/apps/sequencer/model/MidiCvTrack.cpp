#include "MidiCvTrack.h"

#include "ProjectVersion.h"

void MidiCvTrack::writeRouted(Routing::Target target, int intValue, float floatValue) {
    switch (target) {
    case Routing::Target::SlideTime:
        setSlideTime(intValue, true);
        break;
    case Routing::Target::Transpose:
        setTranspose(intValue, true);
        break;
    default:
        break;
    }
}

void MidiCvTrack::clear() {
    _source.clear();
    setVoices(1);
    setVoiceConfig(VoiceConfig::Pitch);
    setNotePriority(NotePriority::LastNote);
    setLowNote(0);
    setHighNote(127);
    setPitchBendRange(2);
    setModulationRange(Types::VoltageRange::Unipolar5V);
    setRetrigger(false);
    setSlideTime(0);
    setTranspose(0);
    _arpeggiator.clear();
}

void MidiCvTrack::gateOutputName(int index, StringBuilder &str) const {
    str("Gate%d", (index % _voices) + 1);
}

void MidiCvTrack::cvOutputName(int index, StringBuilder &str) const {
    int signals = int(_voiceConfig) + 1;
    int totalOutputs = _voices * signals;
    index %= totalOutputs;
    int voiceIndex = index % _voices;
    int signalIndex = index / _voices;
    switch (signalIndex) {
    case 0: str("V/Oct%d", voiceIndex + 1); break;
    case 1: str("Vel%d", voiceIndex + 1); break;
    case 2: str("Press%d", voiceIndex + 1); break;
    }
}

void MidiCvTrack::write(WriteContext &context) const {
    auto &writer = context.writer;
    _source.write(context);
    writer.write(_voices);
    writer.write(_voiceConfig);
    writer.write(_notePriority);
    writer.write(_lowNote);
    writer.write(_highNote);
    writer.write(_pitchBendRange);
    writer.write(_modulationRange);
    writer.write(_retrigger);
    writer.write(_slideTime.base);
    writer.write(_transpose.base);
    _arpeggiator.write(context);
}

void MidiCvTrack::read(ReadContext &context) {
    auto &reader = context.reader;
    _source.read(context);
    reader.read(_voices);
    reader.read(_voiceConfig);
    reader.read(_notePriority, ProjectVersion::Version16);
    reader.read(_lowNote, ProjectVersion::Version15);
    reader.read(_highNote, ProjectVersion::Version15);
    reader.read(_pitchBendRange);
    reader.read(_modulationRange);
    reader.read(_retrigger);
    reader.read(_slideTime.base, ProjectVersion::Version20);
    reader.read(_transpose.base, ProjectVersion::Version21);
    _arpeggiator.read(context);
}
