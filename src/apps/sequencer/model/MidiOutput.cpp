#include "MidiOutput.h"

//----------------------------------------
// MidiOutput::Output
//----------------------------------------

void MidiOutput::Output::clear() {
    _target.clear();
    _event = MidiOutput::Output::Event::None;
    std::memset(&_data, 0, sizeof(_data));
}

void MidiOutput::Output::write(WriteContext &context) const {
    auto &writer = context.writer;

    _target.write(context);
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

void MidiOutput::Output::read(ReadContext &context) {
    auto &reader = context.reader;

    _target.read(context);
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

MidiOutput::MidiOutput(Project &project) :
    _project(project)
{
    clear();
}

void MidiOutput::clear() {
    for (auto &output : _outputs) {
        output.clear();
    }
}

void MidiOutput::write(WriteContext &context) const {
    writeArray(context, _outputs);
}

void MidiOutput::read(ReadContext &context) {
    readArray(context, _outputs);
}
