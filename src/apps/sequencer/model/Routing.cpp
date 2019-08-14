#include "Routing.h"

#include "Project.h"
#include "ProjectVersion.h"

#include <cmath>

//----------------------------------------
// Routing::CvSource
//----------------------------------------

void Routing::CvSource::clear() {
    _range = Types::VoltageRange::Bipolar5V;
}

void Routing::CvSource::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_range);
}

void Routing::CvSource::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_range);
}

bool Routing::CvSource::operator==(const CvSource &other) const {
    return _range == other._range;
}

//----------------------------------------
// Routing::MidiSource
//----------------------------------------

void Routing::MidiSource::clear() {
    _source.clear();
    _event = Event::ControlAbsolute;
    _controlNumberOrNote = 0;
    _noteRange = 2;
}

void Routing::MidiSource::write(WriteContext &context) const {
    auto &writer = context.writer;
    _source.write(context);
    writer.write(_event);
    writer.write(_controlNumberOrNote);
    writer.write(_noteRange);
}

void Routing::MidiSource::read(ReadContext &context) {
    auto &reader = context.reader;
    _source.read(context);
    reader.read(_event);
    reader.read(_controlNumberOrNote);
    reader.read(_noteRange, ProjectVersion::Version13);
}

bool Routing::MidiSource::operator==(const MidiSource &other) const {
    return (
        _source == other._source &&
        _event == other._event &&
        _controlNumberOrNote == other._controlNumberOrNote &&
        (_event != Event::NoteRange || _noteRange == other._noteRange)
    );
}

//----------------------------------------
// Routing::Route
//----------------------------------------

Routing::Route::Route() {
    clear();
}

void Routing::Route::clear() {
    _target = Target::None;
    _tracks = 0;
    _min = 0.f;
    _max = 1.f;
    _source = Source::None;
    _cvSource.clear();
    _midiSource.clear();
}

void Routing::Route::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.writeEnum(_target, targetSerialize);
    writer.write(_tracks);
    writer.write(_min);
    writer.write(_max);
    writer.write(_source);
    if (isCvSource(_source)) {
        _cvSource.write(context);
    }
    if (isMidiSource(_source)) {
        _midiSource.write(context);
    }
}

void Routing::Route::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.readEnum(_target, targetSerialize);
    reader.read(_tracks);
    reader.read(_min);
    reader.read(_max);
    reader.read(_source);
    if (isCvSource(_source)) {
        _cvSource.read(context);
    }
    if (isMidiSource(_source)) {
        _midiSource.read(context);
    }
}

bool Routing::Route::operator==(const Route &other) const {
    return (
        _target == other._target &&
        _tracks == other._tracks &&
        _min == other._min &&
        _max == other._max &&
        _source == other._source &&
        (!isCvSource(_source) || _cvSource == other._cvSource) &&
        (!isMidiSource(_source) || _midiSource == other._midiSource)
    );
}

//----------------------------------------
// Routing
//----------------------------------------

Routing::Routing(Project &project) :
    _project(project)
{}

void Routing::clear() {
    for (auto &route : _routes) {
        route.clear();
    }
}

int Routing::findEmptyRoute() const {
    for (size_t i = 0; i < _routes.size(); ++i) {
        if (!_routes[i].active()) {
            return i;
        }
    }
    return -1;
}

int Routing::findRoute(Target target, int trackIndex) const {
    for (size_t i = 0; i < _routes.size(); ++i) {
        const auto &route = _routes[i];
        if (route.active() && route.target() == target && (!Routing::isTrackTarget(target) || route.tracks() & (1<<trackIndex))) {
            return i;
        }
    }
    return -1;
}

int Routing::checkRouteConflict(const Route &editedRoute, const Route &existingRoute) const {
    for (size_t i = 0; i < _routes.size(); ++i) {
        const auto &route = _routes[i];
        if (&route != &existingRoute && route.active() && route.target() == editedRoute.target()) {
            if (isPerTrackTarget(route.target())) {
                if ((route.tracks() & editedRoute.tracks()) != 0) {
                    return i;
                }
            } else {
                return i;
            }
        }
    }

    return -1;
}

void Routing::writeTarget(Target target, uint8_t tracks, float normalized) {
    float floatValue = denormalizeTargetValue(target, normalized);
    int intValue = std::round(floatValue);

    if (isProjectTarget(target)) {
        _project.writeRouted(target, intValue, floatValue);
    } else if (isPlayStateTarget(target)) {
        _project.playState().writeRouted(target, tracks, intValue, floatValue);
    } else if (isTrackTarget(target) || isSequenceTarget(target)) {
        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            if (tracks & (1<<trackIndex)) {
                auto &track = _project.track(trackIndex);
                switch (track.trackMode()) {
                case Track::TrackMode::Note:
                    if (isTrackTarget(target)) {
                        track.noteTrack().writeRouted(target, intValue, floatValue);
                    } else {
                        for (int patternIndex = 0; patternIndex < CONFIG_PATTERN_COUNT; ++patternIndex) {
                            track.noteTrack().sequence(patternIndex).writeRouted(target, intValue, floatValue);
                        }
                    }
                    break;
                case Track::TrackMode::Curve:
                    if (isTrackTarget(target)) {
                        track.curveTrack().writeRouted(target, intValue, floatValue);
                    } else {
                        for (int patternIndex = 0; patternIndex < CONFIG_PATTERN_COUNT; ++patternIndex) {
                            track.curveTrack().sequence(patternIndex).writeRouted(target, intValue, floatValue);
                        }
                    }
                    break;
                case Track::TrackMode::MidiCv:
                    if (isTrackTarget(target)) {
                        track.midiCvTrack().writeRouted(target, intValue, floatValue);
                    }
                    break;
                case Track::TrackMode::Last:
                    break;
                }
            }
        }
    }
}

void Routing::write(WriteContext &context) const {
    writeArray(context, _routes);
}

void Routing::read(ReadContext &context) {
    readArray(context, _routes);
}

static std::array<uint8_t, size_t(Routing::Target::Last)> routedSet;
static_assert(sizeof(uint8_t) * 8 >= CONFIG_TRACK_COUNT, "track bits do not fit");

bool Routing::isRouted(Target target, int trackIndex) {
    size_t targetIndex = size_t(target);
    if (isPerTrackTarget(target)) {
        if (trackIndex >= 0 && trackIndex < CONFIG_TRACK_COUNT) {
            return (routedSet[targetIndex] & (1 << trackIndex)) != 0;
        }
    } else {
        return routedSet[targetIndex] != 0;
    }
    return false;
}

void Routing::setRouted(Target target, uint8_t tracks, bool routed) {
    size_t targetIndex = size_t(target);
    if (isPerTrackTarget(target)) {
        if (routed) {
            routedSet[targetIndex] |= tracks;
        } else {
            routedSet[targetIndex] &= ~tracks;
        }
    } else {
        routedSet[targetIndex] = routed ? 1 : 0;
    }
}

void Routing::printRouted(StringBuilder &str, Target target, int trackIndex) {
    if (isRouted(target, trackIndex)) {
        str("\x1a");
    }
}

struct TargetInfo {
    int16_t min;
    int16_t max;
    int16_t minDef;
    int16_t maxDef;
};

const TargetInfo targetInfos[int(Routing::Target::Last)] = {
    [int(Routing::Target::None)]                            = { 0,      0,      0,      0       },
    // Engine targets
    [int(Routing::Target::Play)]                            = { 0,      1,      0,      1       },
    [int(Routing::Target::Record)]                          = { 0,      1,      0,      1       },
    [int(Routing::Target::TapTempo)]                        = { 0,      1,      0,      1       },
    // Project targets
    [int(Routing::Target::Tempo)]                           = { 20,     500,    100,    200     },
    [int(Routing::Target::Swing)]                           = { 50,     75,     50,     75      },
    // PlayState targets
    [int(Routing::Target::Mute)]                            = { 0,      1,      0,      1       },
    [int(Routing::Target::Fill)]                            = { 0,      1,      0,      1       },
    [int(Routing::Target::FillAmount)]                      = { 0,      100,    0,      100     },
    [int(Routing::Target::Pattern)]                         = { 0,      15,     0,      15      },
    // Track targets
    [int(Routing::Target::SlideTime)]                       = { 0,      100,    0,      100,    },
    [int(Routing::Target::Octave)]                          = { -10,    10,     -1,     1       },
    [int(Routing::Target::Transpose)]                       = { -60,    60,     -12,    12      },
    [int(Routing::Target::Rotate)]                          = { -64,    64,     0,      64      },
    [int(Routing::Target::GateProbabilityBias)]             = { -8,     8,      -8,     8       },
    [int(Routing::Target::RetriggerProbabilityBias)]        = { -8,     8,      -8,     8       },
    [int(Routing::Target::LengthBias)]                      = { -8,     8,      -8,     8       },
    [int(Routing::Target::NoteProbabilityBias)]             = { -8,     8,      -8,     8       },
    [int(Routing::Target::ShapeProbabilityBias)]            = { -8,     8,      -8,     8       },
    // Sequence targets
    [int(Routing::Target::Divisor)]                         = { 1,      768,    6,      24      },
    [int(Routing::Target::RunMode)]                         = { 0,      5,      0,      5       },
    [int(Routing::Target::FirstStep)]                       = { 0,      63,     0,      63      },
    [int(Routing::Target::LastStep)]                        = { 0,      63,     0,      63      },
};

float Routing::normalizeTargetValue(Routing::Target target, float value) {
    const auto &info = targetInfos[int(target)];
    return clamp((value - info.min) / (info.max - info.min), 0.f, 1.f);
}

float Routing::denormalizeTargetValue(Routing::Target target, float normalized) {
    const auto &info = targetInfos[int(target)];
    return normalized * (info.max - info.min) + info.min;
}

std::pair<float, float> Routing::normalizedDefaultRange(Target target) {
    const auto &info = targetInfos[int(target)];
    return { normalizeTargetValue(target, info.minDef), normalizeTargetValue(target, info.maxDef) };
}

float Routing::targetValueStep(Routing::Target target) {
    const auto &info = targetInfos[int(target)];
    return 1.f / (info.max - info.min);
}

void Routing::printTargetValue(Routing::Target target, float normalized, StringBuilder &str) {
    float value = denormalizeTargetValue(target, normalized);
    int intValue = std::round(value);
    switch (target) {
    case Target::None:
        str("-");
        break;
    case Target::Tempo:
        str("%.1f", value);
        break;
    case Target::Swing:
    case Target::SlideTime:
    case Target::FillAmount:
        str("%d%%", intValue);
        break;
    case Target::Octave:
    case Target::Transpose:
    case Target::Rotate:
        str("%+d", intValue);
        break;
    case Target::GateProbabilityBias:
    case Target::RetriggerProbabilityBias:
    case Target::LengthBias:
    case Target::NoteProbabilityBias:
        str("%+.1f%%", value * 12.5f);
        break;
    case Target::Divisor:
        ModelUtils::printDivisor(str, intValue);
        break;
    case Target::RunMode:
        str("%s", Types::runModeName(Types::RunMode(intValue)));
        break;
    case Target::FirstStep:
    case Target::LastStep:
    case Target::Pattern:
        str("%d", intValue + 1);
        break;
    case Target::Mute:
    case Target::Fill:
        str(intValue ? "on" : "off");
        break;
    default:
        str("%d", intValue);
        break;
    }
}
