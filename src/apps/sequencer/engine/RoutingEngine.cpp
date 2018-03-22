#include "RoutingEngine.h"

#include "Engine.h"

// for allowing direct mapping
static_assert(int(MIDIPort::MIDI) == int(Routing::MIDISource::Port::MIDI), "invalid mapping");
static_assert(int(MIDIPort::USBMIDI) == int(Routing::MIDISource::Port::USBMIDI), "invalid mapping");

RoutingEngine::RoutingEngine(Engine &engine, Model &model) :
    _engine(engine),
    _model(model),
    _project(model.project()),
    _routing(model.project().routing())
{}

void RoutingEngine::update() {
    updateSources();
    updateSinks();
}

void RoutingEngine::receiveMIDI(MIDIPort port, const MIDIMessage &message) {
    for (int i = 0; i < CONFIG_ROUTE_COUNT; ++i) {
        const auto &route = _routing.route(i);
        if (route.active() &&
            route.source().kind() == Routing::Source::Kind::MIDI &&
            route.source().midi().port() == Routing::MIDISource::Port(port) &&
            (route.source().midi().channel() == 0 || route.source().midi().channel() == message.channel() + 1)
        ) {
            const auto &midiSource = route.source().midi();
            auto &sourceValue = _sourceValues[i];
            switch (midiSource.kind()) {
            case Routing::MIDISource::Kind::CCAbsolute:
                if (message.controllerNumber() == midiSource.controller()) {
                    sourceValue = message.controllerValue() * (1.f / 127.f);
                }
                break;
            case Routing::MIDISource::Kind::PitchBend:
                if (message.isPitchBend()) {
                    sourceValue = (message.pitchBend() + 0x2000) * (1.f / 16384.f);
                }
                break;
            case Routing::MIDISource::Kind::NoteMomentary:
                if (message.isNoteOn() && message.note() == midiSource.note()) {
                    sourceValue = 1.f;
                } else if (message.isNoteOff() && message.note() == midiSource.note()) {
                    sourceValue = 0.f;
                }
                break;
            case Routing::MIDISource::Kind::NoteToggle:
                if (message.isNoteOn() && message.note() == midiSource.note()) {
                    sourceValue = sourceValue < 0.5f ? 1.f : 0.f;
                }
                break;
            case Routing::MIDISource::Kind::NoteVelocity:
                if (message.isNoteOn() && message.note() == midiSource.note()) {
                    sourceValue = message.velocity() * (1.f / 127.f);
                }
                break;
            }
        }
    }
}

void RoutingEngine::updateSources() {
    for (int i = 0; i < CONFIG_ROUTE_COUNT; ++i) {
        const auto &route = _routing.route(i);
        if (route.active()) {
            const auto &source = route.source();
            auto &sourceValue = _sourceValues[i];
            switch (source.kind()) {
            case Routing::Source::Kind::CV:
                sourceValue = (_engine.cvInput().channel(source.cv().index()) + 5.f) / 10.f;
                break;
            case Routing::Source::Kind::Track:
                sourceValue = (_engine.cvOutput().channel(source.track().index()) + 5.f) / 10.f;
                break;
            case Routing::Source::Kind::MIDI:
                // handled in receiveMIDI
                break;
            case Routing::Source::Kind::Last:
                break;
            }
        }
    }
}

void RoutingEngine::updateSinks() {
    for (int i = 0; i < CONFIG_ROUTE_COUNT; ++i) {
        const auto &route = _routing.route(i);
        if (route.active()) {
            const auto &sink = route.sink();
            float value = _sourceValues[i];
            // TODO handle pattern
            _routing.writeParam(sink.param(), sink.track(), 0, value);
        }
    }
}
