#include "MidiOutput.h"
#include "ProjectVersion.h"

//----------------------------------------
// MidiOutput::Output
//----------------------------------------

void MidiOutput::Output::clear() {
    _target.clear();
    setEvent(Event::None, true);
}

void MidiOutput::Output::write(VersionedSerializedWriter &writer) const {
    _target.write(writer);
    writer.write(_event);

    switch (_event) {
    case MidiOutput::Output::Event::None:
        break;
    case MidiOutput::Output::Event::Note:
        writer.write(_data.note.gateSource);
        writer.write(_data.note.noteSource);
        writer.write(_data.note.velocitySource);
        break;
    case MidiOutput::Output::Event::ControlChange:
        writer.write(_data.controlChange.controlNumber);
        writer.write(_data.controlChange.controlSource);
        break;
    case MidiOutput::Output::Event::Last:
        break;
    }
}

void MidiOutput::Output::read(VersionedSerializedReader &reader) {
    _target.read(reader);
    reader.read(_event);

    switch (_event) {
    case MidiOutput::Output::Event::None:
        break;
    case MidiOutput::Output::Event::Note:
        reader.read(_data.note.gateSource);
        reader.read(_data.note.noteSource);
        reader.read(_data.note.velocitySource);
        break;
    case MidiOutput::Output::Event::ControlChange:
        reader.read(_data.controlChange.controlNumber);
        reader.read(_data.controlChange.controlSource);
        break;
    case MidiOutput::Output::Event::Last:
        break;
    }
}

bool MidiOutput::Output::operator==(const Output &other) const {
    return (
        _target == other._target &&
        _event == other._event &&
        (_event != MidiOutput::Output::Event::Note || _data.note == other._data.note) &&
        (_event != MidiOutput::Output::Event::ControlChange || _data.controlChange == other._data.controlChange)
    );
}

//----------------------------------------
// MidiOutput
//----------------------------------------

MidiOutput::MidiOutput() {
    clear();
}

void MidiOutput::clear() {
    for (auto &output : _outputs) {
        output.clear();
    }
}

void MidiOutput::write(VersionedSerializedWriter &writer) const {
    writeArray(writer, _outputs);
}

void MidiOutput::read(VersionedSerializedReader &reader) {
    if (reader.dataVersion() < ProjectVersion::Version24) {
        readArray(reader, _outputs, 8);
    } else {
        readArray(reader, _outputs);
    }
}
