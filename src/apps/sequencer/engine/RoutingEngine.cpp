#include "RoutingEngine.h"

#include "Engine.h"

// for allowing direct mapping
static_assert(int(MidiPort::Midi) == int(Types::MidiPort::Midi), "invalid mapping");
static_assert(int(MidiPort::UsbMidi) == int(Types::MidiPort::UsbMidi), "invalid mapping");

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
            route.source() == Routing::Source::Midi &&
            route.midiSource().port() == Types::MidiPort(port) &&
            (route.midiSource().channel() == 0 || route.midiSource().channel() == message.channel() + 1)
        ) {
            const auto &midiSource = route.midiSource();
            auto &sourceValue = _sourceValues[i];
            switch (midiSource.event()) {
            case Routing::MidiSource::Event::ControlAbsolute:
                if (message.controlNumber() == midiSource.controlNumber()) {
                    sourceValue = message.controlValue() * (1.f / 127.f);
                }
                break;
            case Routing::MidiSource::Event::ControlRelative:
                if (message.controlNumber() == midiSource.controlNumber()) {
                    int value = message.controlValue();
                    value = value >= 64 ? 64 - value : value;
                    sourceValue = clamp(sourceValue + value * (1.f / 127.f), 0.f, 1.f);
                }
                break;
            case Routing::MidiSource::Event::PitchBend:
                if (message.isPitchBend()) {
                    sourceValue = (message.pitchBend() + 0x2000) * (1.f / 16384.f);
                }
                break;
            case Routing::MidiSource::Event::NoteMomentary:
                if (message.isNoteOn() && message.note() == midiSource.note()) {
                    sourceValue = 1.f;
                } else if (message.isNoteOff() && message.note() == midiSource.note()) {
                    sourceValue = 0.f;
                }
                break;
            case Routing::MidiSource::Event::NoteToggle:
                if (message.isNoteOn() && message.note() == midiSource.note()) {
                    sourceValue = sourceValue < 0.5f ? 1.f : 0.f;
                }
                break;
            case Routing::MidiSource::Event::NoteVelocity:
                if (message.isNoteOn() && message.note() == midiSource.note()) {
                    sourceValue = message.velocity() * (1.f / 127.f);
                }
                break;
            case Routing::MidiSource::Event::Last:
                break;
            }
        }
    }
}

void RoutingEngine::updateSources() {
    for (int i = 0; i < CONFIG_ROUTE_COUNT; ++i) {
        const auto &route = _routing.route(i);
        if (route.active()) {
            auto &sourceValue = _sourceValues[i];
            switch (route.source()) {
            case Routing::Source::None:
                sourceValue = 0.f;
                break;
            case Routing::Source::CvIn1:
            case Routing::Source::CvIn2:
            case Routing::Source::CvIn3:
            case Routing::Source::CvIn4: {
                int index = int(route.source()) - int(Routing::Source::CvIn1);
                sourceValue = (_engine.cvInput().channel(index) + 5.f) / 10.f;
                break;
            }
            case Routing::Source::CvOut1:
            case Routing::Source::CvOut2:
            case Routing::Source::CvOut3:
            case Routing::Source::CvOut4:
            case Routing::Source::CvOut5:
            case Routing::Source::CvOut6:
            case Routing::Source::CvOut7:
            case Routing::Source::CvOut8: {
                int index = int(route.source()) - int(Routing::Source::CvOut1);
                sourceValue = (_engine.cvOutput().channel(index) + 5.f) / 10.f;
                break;
            }
            case Routing::Source::Midi:
                // handled in receiveMidi
                break;
            case Routing::Source::Last:
                break;
            }
        }
    }
}

void RoutingEngine::updateSinks() {
    for (int i = 0; i < CONFIG_ROUTE_COUNT; ++i) {
        const auto &route = _routing.route(i);
        if (route.active()) {
            float value = route.min() + _sourceValues[i] * (route.max() - route.min());
            // TODO handle pattern
            _routing.writeParam(route.param(), route.track(), 0, value);
        }
    }
}
