#include "RoutingEngine.h"

#include "Engine.h"

// for allowing direct mapping
static_assert(int(MidiPort::Midi) == int(Routing::MidiSource::Port::Midi), "invalid mapping");
static_assert(int(MidiPort::UsbMidi) == int(Routing::MidiSource::Port::UsbMidi), "invalid mapping");

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

void RoutingEngine::receiveMidi(MidiPort port, const MidiMessage &message) {
    for (int i = 0; i < CONFIG_ROUTE_COUNT; ++i) {
        const auto &route = _routing.route(i);
        if (route.active() &&
            route.source().kind() == Routing::Source::Kind::Midi &&
            route.source().midi().port() == Routing::MidiSource::Port(port) &&
            (route.source().midi().channel() == 0 || route.source().midi().channel() == message.channel() + 1)
        ) {
            const auto &midiSource = route.source().midi();
            auto &sourceValue = _sourceValues[i];
            switch (midiSource.kind()) {
            case Routing::MidiSource::Kind::ControllerAbs:
                if (message.controllerNumber() == midiSource.controller()) {
                    sourceValue = message.controllerValue() * (1.f / 127.f);
                }
                break;
            case Routing::MidiSource::Kind::ControllerRel:
                if (message.controllerNumber() == midiSource.controller()) {
                    int value = message.controllerValue();
                    value = value >= 64 ? 64 - value : value;
                    sourceValue = clamp(sourceValue + value * (1.f / 127.f), 0.f, 1.f);
                }
                break;
            case Routing::MidiSource::Kind::PitchBend:
                if (message.isPitchBend()) {
                    sourceValue = (message.pitchBend() + 0x2000) * (1.f / 16384.f);
                }
                break;
            case Routing::MidiSource::Kind::NoteMomentary:
                if (message.isNoteOn() && message.note() == midiSource.note()) {
                    sourceValue = 1.f;
                } else if (message.isNoteOff() && message.note() == midiSource.note()) {
                    sourceValue = 0.f;
                }
                break;
            case Routing::MidiSource::Kind::NoteToggle:
                if (message.isNoteOn() && message.note() == midiSource.note()) {
                    sourceValue = sourceValue < 0.5f ? 1.f : 0.f;
                }
                break;
            case Routing::MidiSource::Kind::NoteVelocity:
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
            case Routing::Source::Kind::None:
                sourceValue = 0.f;
                break;
            case Routing::Source::Kind::CV:
                sourceValue = (_engine.cvInput().channel(source.cv().index()) + 5.f) / 10.f;
                break;
            case Routing::Source::Kind::Track:
                sourceValue = (_engine.cvOutput().channel(source.track().index()) + 5.f) / 10.f;
                break;
            case Routing::Source::Kind::Midi:
                // handled in receiveMidi
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
        if (route.active() && route.hasSource()) {
            float value = _sourceValues[i];
            // TODO handle pattern
            _routing.writeParam(route.param(), route.track(), 0, value);
        }
    }
}
