#include "MidiCvTrack.h"

void MidiCvTrack::clear() {
    _port = Types::MidiPort::Midi;
    _channel = -1;
    _voices = 1;
}

void MidiCvTrack::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_port);
    writer.write(_channel);
}

void MidiCvTrack::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_port);
    reader.read(_channel);
}
