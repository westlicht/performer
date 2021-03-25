#include "MidiCvTrack.h"

#include "ProjectVersion.h"

struct VoiceConfigInfo {
    uint8_t signalCount;
    MidiCvTrack::VoiceSignal signals[3];
};

static const struct VoiceConfigInfo voiceConfigInfos[int(MidiCvTrack::VoiceConfig::Last)] = {
    [int(MidiCvTrack::VoiceConfig::Pitch)]                  = { 1, { MidiCvTrack::VoiceSignal::Pitch } },
    [int(MidiCvTrack::VoiceConfig::Velocity)]               = { 1, { MidiCvTrack::VoiceSignal::Velocity } },
    [int(MidiCvTrack::VoiceConfig::PitchVelocity)]          = { 2, { MidiCvTrack::VoiceSignal::Pitch, MidiCvTrack::VoiceSignal::Velocity } },
    [int(MidiCvTrack::VoiceConfig::PitchVelocityPressure)]  = { 3, { MidiCvTrack::VoiceSignal::Pitch, MidiCvTrack::VoiceSignal::Velocity, MidiCvTrack::VoiceSignal::Pressure } },
};

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

int MidiCvTrack::voiceSignalCount() const {
    const auto &info = voiceConfigInfos[int(_voiceConfig)];
    return info.signalCount;
}

MidiCvTrack::VoiceSignal MidiCvTrack::voiceSignalByIndex(int index) const {
    const auto &info = voiceConfigInfos[int(_voiceConfig)];
    return info.signals[index % info.signalCount];
}

void MidiCvTrack::gateOutputName(int index, StringBuilder &str) const {
    str("Gate%d", (index % _voices) + 1);
}

void MidiCvTrack::cvOutputName(int index, StringBuilder &str) const {
    int signalCount = voiceSignalCount();
    int totalOutputs = _voices * signalCount;
    index %= totalOutputs;
    int voiceIndex = index % _voices;
    int signalIndex = index / _voices;
    switch (voiceSignalByIndex(signalIndex)) {
    case VoiceSignal::Pitch:
        str("V/Oct%d", voiceIndex + 1);
        break;
    case VoiceSignal::Velocity:
        str("Vel%d", voiceIndex + 1);
        break;
    case VoiceSignal::Pressure:
        str("Press%d", voiceIndex + 1);
        break;
    }
}

void MidiCvTrack::write(VersionedSerializedWriter &writer) const {
    _source.write(writer);
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
    _arpeggiator.write(writer);
}

void MidiCvTrack::read(VersionedSerializedReader &reader) {
    _source.read(reader);
    reader.read(_voices);
    if (reader.dataVersion() < ProjectVersion::Version31) {
        uint32_t voiceConfig;
        reader.read(voiceConfig);
        switch (voiceConfig) {
        case 0: _voiceConfig = VoiceConfig::Pitch; break;
        case 1: _voiceConfig = VoiceConfig::PitchVelocity; break;
        case 2: _voiceConfig = VoiceConfig::PitchVelocityPressure; break;
        default: _voiceConfig = VoiceConfig::Pitch; break;
        }
    } else {
        reader.read(_voiceConfig);
    }
    reader.read(_notePriority, ProjectVersion::Version16);
    reader.read(_lowNote, ProjectVersion::Version15);
    reader.read(_highNote, ProjectVersion::Version15);
    reader.read(_pitchBendRange);
    reader.read(_modulationRange);
    reader.read(_retrigger);
    reader.read(_slideTime.base, ProjectVersion::Version20);
    reader.read(_transpose.base, ProjectVersion::Version21);
    _arpeggiator.read(reader);
}
