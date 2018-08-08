#include "MidiCvTrack.h"

void MidiCvTrack::clear() {
    _source.clear();
    setVoices(1);
    setVoiceConfig(VoiceConfig::Pitch);
    setPitchBendRange(2);
    setModulationRange(Types::VoltageRange::Unipolar5V);
    setRetrigger(false);
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
    writer.write(_pitchBendRange);
    writer.write(_modulationRange);
    writer.write(_retrigger);
}

void MidiCvTrack::read(ReadContext &context) {
    auto &reader = context.reader;
    _source.read(context);
    reader.read(_voices);
    reader.read(_voiceConfig);
    reader.read(_pitchBendRange);
    reader.read(_modulationRange);
    reader.read(_retrigger);
}
