#include "MidiCvTrack.h"

void MidiCvTrack::clear() {
    _port = Types::MidiPort::Midi;
    _channel = -1;
    _voices = 1;
    _outputVelocity = false;
    _outputPressure = false;
}

void MidiCvTrack::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_port);
    writer.write(_channel);
    writer.write(_voices);
    writer.write(_outputVelocity);
    writer.write(_outputPressure);
}

void MidiCvTrack::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_port);
    reader.read(_channel);
    reader.read(_voices);
    reader.read(_outputVelocity);
    reader.read(_outputPressure);
}
