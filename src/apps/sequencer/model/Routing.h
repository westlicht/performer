#pragma once

#include "Config.h"

#include "Types.h"
#include "MidiConfig.h"
#include "Serialize.h"
#include "ModelUtils.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>
#include <bitset>

#include <cstdint>

class Project;
class NoteSequence;
class CurveSequence;

class Routing {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    enum class Target : uint8_t {
        None,
        // Engine targets
        EngineFirst,
        Play = EngineFirst,
        PlayToggle,
        Record,
        RecordToggle,
        TapTempo,
        EngineLast = TapTempo,

        // Project targets
        ProjectFirst,
        Tempo = ProjectFirst,
        Swing,
        ProjectLast = Swing,

        // PlayState targets
        PlayStateFirst,
        Mute = PlayStateFirst,
        Fill,
        FillAmount,
        Pattern,
        PlayStateLast = Pattern,

        // Track targets
        TrackFirst,
        SlideTime = TrackFirst,
        Octave,
        Transpose,
        Offset,
        Rotate,
        GateProbabilityBias,
        RetriggerProbabilityBias,
        LengthBias,
        NoteProbabilityBias,
        ShapeProbabilityBias,
        TrackLast = ShapeProbabilityBias,

        // Sequence targets
        SequenceFirst,
        FirstStep = SequenceFirst,
        LastStep,
        RunMode,
        Divisor,
        Scale,
        RootNote,
        SequenceLast = RootNote,

        Last,
    };

    static const char *targetName(Target target) {
        switch (target) {
        case Target::None:                      return "None";

        case Target::Play:                      return "Play";
        case Target::PlayToggle:                return "Play Toggle";
        case Target::Record:                    return "Record";
        case Target::RecordToggle:              return "Record Toggle";
        case Target::TapTempo:                  return "Tap Tempo";

        case Target::Tempo:                     return "Tempo";
        case Target::Swing:                     return "Swing";

        case Target::Mute:                      return "Mute";
        case Target::Fill:                      return "Fill";
        case Target::FillAmount:                return "Fill Amount";
        case Target::Pattern:                   return "Pattern";

        case Target::SlideTime:                 return "Slide Time";
        case Target::Octave:                    return "Octave";
        case Target::Transpose:                 return "Transpose";
        case Target::Offset:                    return "Offset";
        case Target::Rotate:                    return "Rotate";
        case Target::GateProbabilityBias:       return "Gate P. Bias";
        case Target::RetriggerProbabilityBias:  return "Retrig P. Bias";
        case Target::LengthBias:                return "Length Bias";
        case Target::NoteProbabilityBias:       return "Note P. Bias";
        case Target::ShapeProbabilityBias:      return "Shape P. Bias";

        case Target::FirstStep:                 return "First Step";
        case Target::LastStep:                  return "Last Step";
        case Target::RunMode:                   return "Run Mode";
        case Target::Divisor:                   return "Divisor";
        case Target::Scale:                     return "Scale";
        case Target::RootNote:                  return "Root Note";

        case Target::Last:                      break;
        }
        return nullptr;
    }

    static uint8_t targetSerialize(Target target) {
        switch (target) {
        case Target::None:                      return 0;
        case Target::Play:                      return 1;
        case Target::Record:                    return 2;
        case Target::Tempo:                     return 3;
        case Target::Swing:                     return 4;
        case Target::SlideTime:                 return 5;
        case Target::Octave:                    return 6;
        case Target::Transpose:                 return 7;
        case Target::Rotate:                    return 8;
        case Target::GateProbabilityBias:       return 9;
        case Target::RetriggerProbabilityBias:  return 10;
        case Target::LengthBias:                return 11;
        case Target::NoteProbabilityBias:       return 12;
        case Target::Divisor:                   return 13;
        case Target::RunMode:                   return 14;
        case Target::FirstStep:                 return 15;
        case Target::LastStep:                  return 16;

        case Target::Mute:                      return 17;
        case Target::Fill:                      return 18;
        case Target::FillAmount:                return 19;
        case Target::Pattern:                   return 20;

        case Target::TapTempo:                  return 21;

        case Target::ShapeProbabilityBias:      return 22;

        case Target::Scale:                     return 23;
        case Target::RootNote:                  return 24;

        case Target::Offset:                    return 25;

        case Target::PlayToggle:                return 26;
        case Target::RecordToggle:              return 27;

        case Target::Last:                      break;
        }
        return 0;
    }

    static bool isEngineTarget(Target target) {
        return target >= Target::EngineFirst && target <= Target::EngineLast;
    }

    static bool isProjectTarget(Target target) {
        return target >= Target::ProjectFirst && target <= Target::ProjectLast;
    }

    static bool isPlayStateTarget(Target target) {
        return target >= Target::PlayStateFirst && target <= Target::PlayStateLast;
    }

    static bool isTrackTarget(Target target) {
        return target >= Target::TrackFirst && target <= Target::TrackLast;
    }

    static bool isSequenceTarget(Target target) {
        return target >= Target::SequenceFirst && target <= Target::SequenceLast;
    }

    static bool isPerTrackTarget(Target target) {
        return isPlayStateTarget(target) || isTrackTarget(target) || isSequenceTarget(target);
    }

    enum class Source : uint8_t {
        None,
        CvIn1,
        CvFirst = CvIn1,
        CvIn2,
        CvIn3,
        CvIn4,
        CvOut1,
        CvOut2,
        CvOut3,
        CvOut4,
        CvOut5,
        CvOut6,
        CvOut7,
        CvOut8,
        CvLast = CvOut8,
        Midi,
        Last
    };

    static bool isCvSource(Source source) { return source >= Source::CvFirst && source <= Source::CvLast; }
    static bool isMidiSource(Source source) { return source == Source::Midi; }

    static void printSource(Source source, StringBuilder &str) {
        switch (source) {
        case Source::None:
            str("None");
            break;
        case Source::CvIn1:
        case Source::CvIn2:
        case Source::CvIn3:
        case Source::CvIn4:
            str("CV In %d", int(source) - int(Source::CvIn1) + 1);
            break;
        case Source::CvOut1:
        case Source::CvOut2:
        case Source::CvOut3:
        case Source::CvOut4:
        case Source::CvOut5:
        case Source::CvOut6:
        case Source::CvOut7:
        case Source::CvOut8:
            str("CV Out %d", int(source) - int(Source::CvOut1) + 1);
            break;
        case Source::Midi:
            str("MIDI");
        case Source::Last:
            break;
        }
    }

    class CvSource {
    public:
        // range

        Types::VoltageRange range() const { return _range; }
        void setRange(Types::VoltageRange range) {
            _range = ModelUtils::clampedEnum(range);
        }

        void editRange(int value, bool shift) {
            setRange(ModelUtils::adjustedEnum(range(), value));
        }

        void printRange(StringBuilder &str) const {
            str(Types::voltageRangeName(range()));
        }

        void clear();

        void write(VersionedSerializedWriter &writer) const;
        void read(VersionedSerializedReader &reader);

        bool operator==(const CvSource &other) const;

    private:
        Types::VoltageRange _range;
    };

    class MidiSource {
    public:
        enum class Event : uint8_t {
            ControlAbsolute,
            ControlRelative,
            LastControlEvent = ControlRelative,
            PitchBend,
            NoteMomentary,
            NoteToggle,
            NoteVelocity,
            NoteRange,
            Last,
        };

        static const char *eventName(Event event) {
            switch (event) {
            case Event::ControlAbsolute:return "CC Absolute";
            case Event::ControlRelative:return "CC Relative";
            case Event::PitchBend:      return "Pitch Bend";
            case Event::NoteMomentary:  return "Note Momentary";
            case Event::NoteToggle:     return "Note Toggle";
            case Event::NoteVelocity:   return "Note Velocity";
            case Event::NoteRange:      return "Note Range";
            case Event::Last:           break;
            }
            return nullptr;
        }

        // source

        const MidiSourceConfig &source() const { return _source; }
              MidiSourceConfig &source()       { return _source; }

        // event

        Event event() const { return _event; }
        void setEvent(Event event) {
            _event = ModelUtils::clampedEnum(event);
        }

        void editEvent(int value, bool shift) {
            setEvent(ModelUtils::adjustedEnum(event(), value));
        }

        void printEvent(StringBuilder &str) const {
            str(eventName(event()));
        }

        bool isControlEvent() const {
            return int(_event) <= int(Event::LastControlEvent);
        }

        // controlNumber

        int controlNumber() const { return _controlNumberOrNote; }
        void setControlNumber(int controlNumber) {
            _controlNumberOrNote = clamp(controlNumber, 0, 127);
        }

        void editControlNumber(int value, bool shift) {
            setControlNumber(controlNumber() + value);
        }

        void printControlNumber(StringBuilder &str) const {
            str("%d", note());
        }

        // note

        int note() const { return _controlNumberOrNote; }
        void setNote(int note) {
            _controlNumberOrNote = clamp(note, 0, 127);
        }

        void editNote(int value, bool shift) {
            setNote(note() + value);
        }

        void printNote(StringBuilder &str) const {
            Types::printMidiNote(str, note());
        }

        // noteRange

        int noteRange() const { return _noteRange; }
        void setNoteRange(int noteRange) {
            _noteRange = clamp(noteRange, 2, 64);
        }

        void editNoteRange(int value, bool shift) {
            setNoteRange(noteRange() + value);
        }

        void printNoteRange(StringBuilder &str) const {
            str("%d", noteRange());
        }

        void clear();

        void write(VersionedSerializedWriter &writer) const;
        void read(VersionedSerializedReader &reader);

        bool operator==(const MidiSource &other) const;

    private:
        MidiSourceConfig _source;
        Event _event;
        uint8_t _controlNumberOrNote;
        uint8_t _noteRange;
    };

    class Route {
    public:
        // target

        Target target() const { return _target; }
        void setTarget(Target target) {
            target = ModelUtils::clampedEnum(target);
            if (target != _target) {
                _target = target;
                std::tie(_min, _max) = normalizedDefaultRange(target);
            }
        }

        void editTarget(int value, bool shift) {
            setTarget(ModelUtils::adjustedEnum(target(), value));
        }

        void printTarget(StringBuilder &str) const {
            str(targetName(target()));
        }

        // tracks

        uint8_t tracks() const { return isPerTrackTarget(_target) ? _tracks : 0; }
        void setTracks(uint8_t tracks) {
            if (isPerTrackTarget(_target)) {
                _tracks = tracks;
            }
        }

        void toggleTrack(int trackIndex) {
            uint8_t trackBit = (1<<trackIndex);
            if (tracks() & trackBit) {
                setTracks(tracks() & ~trackBit);
            } else {
                setTracks(tracks() | trackBit);
            }
        }

        void printTracks(StringBuilder &str) const {
            if (isPerTrackTarget(_target)) {
                for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
                    str("%c", (_tracks & (1<<i)) ? 'X' : '-');
                }
            } else {
                str("n/a");
            }
        }

        // min

        float min() const { return _min; }
        void setMin(float min) {
            _min = clamp(min, 0.f, 1.f);
            if (max() < _min) {
                setMax(_min);
            }
        }

        void editMin(int value, bool shift) {
            setMin(min() + value * targetValueStep(_target, shift));
        }

        void printMin(StringBuilder &str) const {
            Routing::printTargetValue(_target, _min, str);
        }

        // max

        float max() const { return _max; }
        void setMax(float max) {
            _max = clamp(max, 0.f, 1.f);
            if (min() > _max) {
                setMin(_max);
            }
        }

        void editMax(int value, bool shift) {
            setMax(max() + value * targetValueStep(_target, shift));
        }

        void printMax(StringBuilder &str) const {
            Routing::printTargetValue(_target, _max, str);
        }

        // source

        Source source() const { return _source; }
        void setSource(Source source) {
            _source = ModelUtils::clampedEnum(source);
        }

        void editSource(int value, bool shift) {
            setSource(ModelUtils::adjustedEnum(source(), value));
        }

        void printSource(StringBuilder &str) const {
            Routing::printSource(source(), str);
        }

        // cvSource

        const CvSource &cvSource() const { return _cvSource; }
              CvSource &cvSource()       { return _cvSource; }

        // midiSource

        const MidiSource &midiSource() const { return _midiSource; }
              MidiSource &midiSource()       { return _midiSource; }

        Route();

        void clear();

        bool active() const { return _target != Target::None; }

        void write(VersionedSerializedWriter &writer) const;
        void read(VersionedSerializedReader &reader);

        bool operator==(const Route &other) const;
        bool operator!=(const Route &other) const {
            return !(*this == other);
        }

    private:
        Target _target;
        int8_t _tracks;
        float _min; // TODO make these int16_t
        float _max;
        Source _source;
        CvSource _cvSource;
        MidiSource _midiSource;

        friend class Routing;
    };

    typedef std::array<Route, CONFIG_ROUTE_COUNT> RouteArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // routes

    const RouteArray &routes() const { return _routes; }
          RouteArray &routes()       { return _routes; }

    const Route &route(int index) const { return _routes[index]; }
          Route &route(int index)       { return _routes[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    Routing(Project &project);

    void clear();

    int findEmptyRoute() const;
    int findRoute(Target target, int trackIndex) const;
    int checkRouteConflict(const Route &editedRoute, const Route &existingRoute) const;

    void writeTarget(Target target, uint8_t tracks, float normalized);

    void write(VersionedSerializedWriter &writer) const;
    void read(VersionedSerializedReader &reader);

    bool isDirty() const { return _dirty; }
    void clearDirty() { _dirty = false; }

    // global state for keeping active set of routed targets
    static bool isRouted(Target target, int trackIndex = -1);
    static void setRouted(Target target, uint8_t tracks, bool routed);
    static void printRouted(StringBuilder &str, Target target, int trackIndex = -1);

private:
    static float normalizeTargetValue(Target target, float value);
    static float denormalizeTargetValue(Target target, float normalized);
    static std::pair<float, float> normalizedDefaultRange(Target target);
    static float targetValueStep(Target target, bool shift);
    static void printTargetValue(Target target, float normalized, StringBuilder &str);

    Project &_project;
    RouteArray _routes;
    bool _dirty;
};

// Routable parameters store both a base and routed value.
template<typename T>
struct Routable {
    union {
        struct {
            T base;
            T routed;
        };
        T values[2];
    };

    inline void set(T value, bool selectRouted) { values[selectRouted] = value; }
    inline T get(bool selectRouted) const { return values[selectRouted]; }
};
