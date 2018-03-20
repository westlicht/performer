#include "Routing.h"

//----------------------------------------
// Routing::CVSource
//----------------------------------------

void Routing::CVSource::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_index);
}

void Routing::CVSource::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_index);
}

//----------------------------------------
// Routing::TrackSource
//----------------------------------------

void Routing::TrackSource::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_index);
}

void Routing::TrackSource::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_index);
}

//----------------------------------------
// Routing::MIDISource
//----------------------------------------

void Routing::MIDISource::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_port);
    writer.write(_channel);
    writer.write(_kind);
    writer.write(_data);
}

void Routing::MIDISource::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_port);
    reader.read(_channel);
    reader.read(_kind);
    reader.read(_data);
}

//----------------------------------------
// Routing::Source
//----------------------------------------

void Routing::Source::clear() {
    _kind = Kind::Last;
}

void Routing::Source::initCV(int index) {
    _kind = Kind::CV;
    _source.cv.setIndex(index);
}

void Routing::Source::initTrack(int index) {
    _kind = Kind::Track;
    _source.track.setIndex(index);
}

void Routing::Source::initMIDI() {
    _kind = Kind::MIDI;
    // ...
}

void Routing::Source::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_kind);
    switch (_kind) {
    case Kind::CV:
        _source.cv.write(context);
        break;
    case Kind::Track:
        _source.track.write(context);
        break;
    case Kind::MIDI:
        _source.midi.write(context);
        break;
    case Kind::Last:
        break;
    }
}

void Routing::Source::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_kind);
    switch (_kind) {
    case Kind::CV:
        _source.cv.read(context);
        break;
    case Kind::Track:
        _source.track.read(context);
        break;
    case Kind::MIDI:
        _source.midi.read(context);
        break;
    case Kind::Last:
        break;
    }
}

//----------------------------------------
// Routing::Sink
//----------------------------------------

void Routing::Sink::clear() {
}

void Routing::Sink::init(Param param) {
    _param = param;
    _track = -1;
}

void Routing::Sink::initTrack(Param param, int track) {
    _param = param;
    _track = track;
}

void Routing::Sink::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_param);
    writer.write(_track);
}

void Routing::Sink::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_param);
    reader.read(_track);
}

//----------------------------------------
// Routing::Route
//----------------------------------------

void Routing::Route::clear() {
    _active = false;
    _source.clear();
    _sink.clear();
}

void Routing::Route::write(WriteContext &context) const {
    _source.write(context);
    _sink.write(context);
}

void Routing::Route::read(ReadContext &context) {
    _source.read(context);
    _sink.read(context);
}

//----------------------------------------
// Routing
//----------------------------------------

void Routing::clear() {
    for (auto &route : _routes) {
        route.clear();
    }

    // route(0).source().initCV(0);
    // route(0).sink().init(Routing::Param::BPM);
    // route(0).sink().initTrack(Routing::Param::FirstStep, 0);
    // route(0).enable();

    // route(1).source().initCV(1);
    // route(1).sink().init(Routing::Param::Swing);
    // route(1).sink().initTrack(Routing::Param::LastStep, 0);
    // route(1).enable();
}

void Routing::write(WriteContext &context) const {
    writeArray(context, _routes);
}

void Routing::read(ReadContext &context) {
    readArray(context, _routes);
}
