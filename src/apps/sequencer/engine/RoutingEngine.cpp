#include "RoutingEngine.h"

#include "Engine.h"
#include "MidiUtils.h"

// for allowing direct mapping
static_assert(int(MidiPort::Midi) == int(Types::MidiPort::Midi), "invalid mapping");
static_assert(int(MidiPort::UsbMidi) == int(Types::MidiPort::UsbMidi), "invalid mapping");

RoutingEngine::RoutingEngine(Engine &engine, Model &model) :
    _engine(engine),
    _routing(model.project().routing())
{}

void RoutingEngine::update() {
    updateSources();
    updateSinks();
}

bool RoutingEngine::receiveMidi(MidiPort port, const MidiMessage &message) {
    bool consumed = false;

    for (int routeIndex = 0; routeIndex < CONFIG_ROUTE_COUNT; ++routeIndex) {
        const auto &route = _routing.route(routeIndex);
        if (route.active() &&
            route.source() == Routing::Source::Midi &&
            MidiUtils::matchSource(port, message, route.midiSource().source())
        ) {
            const auto &midiSource = route.midiSource();
            auto &sourceValue = _sourceValues[routeIndex];
            switch (midiSource.event()) {
            case Routing::MidiSource::Event::ControlAbsolute:
                if (message.controlNumber() == midiSource.controlNumber()) {
                    sourceValue = message.controlValue() * (1.f / 127.f);
                    consumed = true;
                }
                break;
            case Routing::MidiSource::Event::ControlRelative:
                if (message.controlNumber() == midiSource.controlNumber()) {
                    int value = message.controlValue();
                    value = value >= 64 ? 64 - value : value;
                    sourceValue = clamp(sourceValue + value * (1.f / 127.f), 0.f, 1.f);
                    consumed = true;
                }
                break;
            case Routing::MidiSource::Event::PitchBend:
                if (message.isPitchBend()) {
                    sourceValue = (message.pitchBend() + 0x2000) * (1.f / 16383.f);
                    consumed = true;
                }
                break;
            case Routing::MidiSource::Event::NoteMomentary:
                if (message.isNoteOn() && message.note() == midiSource.note()) {
                    sourceValue = 1.f;
                    consumed = true;
                } else if (message.isNoteOff() && message.note() == midiSource.note()) {
                    sourceValue = 0.f;
                    consumed = true;
                }
                break;
            case Routing::MidiSource::Event::NoteToggle:
                if (message.isNoteOn() && message.note() == midiSource.note()) {
                    sourceValue = sourceValue < 0.5f ? 1.f : 0.f;
                    consumed = true;
                }
                break;
            case Routing::MidiSource::Event::NoteVelocity:
                if (message.isNoteOn() && message.note() == midiSource.note()) {
                    sourceValue = message.velocity() * (1.f / 127.f);
                    consumed = true;
                }
                break;
            case Routing::MidiSource::Event::NoteRange:
                if (message.isNoteOn() && message.note() >= midiSource.note() && message.note() < midiSource.note() + midiSource.noteRange()) {
                    sourceValue = (message.note() - midiSource.note()) / float(midiSource.noteRange() - 1);
                    consumed = true;
                }
                break;
            case Routing::MidiSource::Event::Last:
                break;
            }
        }
    }

    return consumed;
}

void RoutingEngine::updateSources() {
    for (int routeIndex = 0; routeIndex < CONFIG_ROUTE_COUNT; ++routeIndex) {
        const auto &route = _routing.route(routeIndex);
        if (route.active()) {
            auto &sourceValue = _sourceValues[routeIndex];
            switch (route.source()) {
            case Routing::Source::None:
                sourceValue = 0.f;
                break;
            case Routing::Source::CvIn1:
            case Routing::Source::CvIn2:
            case Routing::Source::CvIn3:
            case Routing::Source::CvIn4: {
                const auto &range = Types::voltageRangeInfo(route.cvSource().range());
                int index = int(route.source()) - int(Routing::Source::CvIn1);
                sourceValue = range.normalize(_engine.cvInput().channel(index));
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
                const auto &range = Types::voltageRangeInfo(route.cvSource().range());
                int index = int(route.source()) - int(Routing::Source::CvOut1);
                sourceValue = range.normalize(_engine.cvOutput().channel(index));
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
    for (int routeIndex = 0; routeIndex < CONFIG_ROUTE_COUNT; ++routeIndex) {
        const auto &route = _routing.route(routeIndex);
        auto &routeState = _routeStates[routeIndex];

        bool routeChanged = route.target() != routeState.target || route.tracks() != routeState.tracks;

        if (routeChanged) {
            // disable previous routing
            Routing::setRouted(routeState.target, routeState.tracks, false);
            // reset last state for play/record toggle
            if (routeState.target == Routing::Target::PlayToggle) {
                _lastPlayToggleActive = false;
            }
            if (routeState.target == Routing::Target::RecordToggle) {
                _lastRecordToggleActive = false;
            }
        }

        if (route.active()) {
            auto target = route.target();
            float value = route.min() + _sourceValues[routeIndex] * (route.max() - route.min());
            if (Routing::isEngineTarget(target)) {
                writeEngineTarget(target, value);
            } else {
                _routing.writeTarget(target, route.tracks(), value);
            }
        }

        if (routeChanged) {
            // enable new routing
            Routing::setRouted(route.target(), route.tracks(), true);
            // save state
            routeState.target = route.target();
            routeState.tracks = route.tracks();
        }
    }
}

void RoutingEngine::writeEngineTarget(Routing::Target target, float normalized) {
    bool active = normalized > 0.5f;

    switch (target) {
    case Routing::Target::Play:
        if (active != _engine.clockRunning()) {
            _engine.togglePlay();
        }
        break;
    case Routing::Target::PlayToggle:
        if (active != _lastPlayToggleActive) {
            if (active) {
                _engine.togglePlay();
            }
            _lastPlayToggleActive = active;
        }
        break;
    case Routing::Target::Record:
        if (active != _engine.recording()) {
            _engine.toggleRecording();
        }
        break;
    case Routing::Target::RecordToggle:
        if (active != _lastRecordToggleActive) {
            if (active) {
                _engine.toggleRecording();
            }
            _lastRecordToggleActive = active;
        }
        break;
    case Routing::Target::TapTempo:
        {
            static bool lastActive = false;
            if (active != lastActive) {
                if (active) {
                    _engine.tapTempoTap();
                }
                lastActive = active;
            }
        }
        break;
    default:
        break;
    }
}
