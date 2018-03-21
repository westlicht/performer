#include "RoutingEngine.h"

#include "Engine.h"

// for allowing direct mapping
static_assert(int(MIDIPort::MIDI) == int(Routing::MIDISource::Port::MIDI), "invalid mapping");
static_assert(int(MIDIPort::USBMIDI) == int(Routing::MIDISource::Port::USBMIDI), "invalid mapping");

struct ParamInfo {
    uint16_t min;
    uint16_t max;
};

const ParamInfo paramInfos[int(Routing::Param::Last)] = {
    [int(Routing::Param::BPM)]              = { 20,     500 },
    [int(Routing::Param::Swing)]            = { 50,     75  },
    [int(Routing::Param::SequenceParams)]   = { 0,      0   },
    [int(Routing::Param::FirstStep)]        = { 0,      63  },
    [int(Routing::Param::LastStep)]         = { 0,      63  },
};

static float normalizeParamValue(Routing::Param param, float value) {
    const auto &info = paramInfos[int(param)];
    return clamp((value - info.min) / (info.max - info.min), 0.f, 1.f);
}

static float denormalizeParamValue(Routing::Param param, float normalized) {
    const auto &info = paramInfos[int(param)];
    return normalized * (info.max - info.min) + info.min;
}

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
            writeParam(sink.param(), sink.track(), 0, value);
        }
    }
}

void RoutingEngine::writeParam(Routing::Param param, int track, int pattern, float value) {
    value = denormalizeParamValue(param, value);
    switch (param) {
    case Routing::Param::BPM:
        _project.setBpm(value);
        break;
    case Routing::Param::Swing:
        _project.setSwing(value);
        break;
    default:
        writeTrackParam(param, track, pattern, value);
        break;
    }
}

void RoutingEngine::writeTrackParam(Routing::Param param, int track, int pattern, float value) {
    auto &sequence = _project.sequence(track, pattern);
    switch (_project.track(track).trackMode()) {
    case Types::TrackMode::Note:
        writeNoteSequenceParam(sequence.noteSequence(), param, value);
        break;
    case Types::TrackMode::Curve:
        writeCurveSequenceParam(sequence.curveSequence(), param, value);
        break;
    case Types::TrackMode::Last:
        break;
    }
}

void RoutingEngine::writeNoteSequenceParam(NoteSequence &sequence, Routing::Param param, float value) {
    switch (param) {
    case Routing::Param::FirstStep:
        sequence.setFirstStep(value);
        break;
    case Routing::Param::LastStep:
        sequence.setLastStep(value);
        break;
    default:
        break;
    }
}

void RoutingEngine::writeCurveSequenceParam(CurveSequence &sequence, Routing::Param param, float value) {

}

float RoutingEngine::readParam(Routing::Param param, int pattern, int track) const {
    switch (param) {
    case Routing::Param::BPM:
        return _project.bpm();
    case Routing::Param::Swing:
        return _project.swing();
    default:
        return 0.f;
    }
}
