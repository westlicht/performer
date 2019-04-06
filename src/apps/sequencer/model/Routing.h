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
        Record,
        EngineLast = Record,

        // Project targets
        ProjectFirst,
        Tempo = ProjectFirst,
        Swing,
        ProjectLast = Swing,

        // Track targets
        TrackFirst,
        SlideTime = TrackFirst,
        Octave,
        Transpose,
        Rotate,
        GateProbabilityBias,
        RetriggerProbabilityBias,
        LengthBias,
        NoteProbabilityBias,
        TrackLast = NoteProbabilityBias,

        // Sequence targets
        SequenceFirst,
        RunMode = SequenceFirst,
        FirstStep,
        LastStep,
        SequenceLast = LastStep,

        Last,
    };

    static const char *targetName(Target target) {
        switch (target) {
        case Target::None:              return "None";

        case Target::Play:              return "Play";
        case Target::Record:            return "Record";

        case Target::Tempo:             return "Tempo";
        case Target::Swing:             return "Swing";

        case Target::SlideTime:         return "Slide Time";
        case Target::Octave:            return "Octave";
        case Target::Transpose:         return "Transpose";
        case Target::Rotate:            return "Rotate";
        case Target::GateProbabilityBias: return "Gate P. Bias";
        case Target::RetriggerProbabilityBias: return "Retrig P. Bias";
        case Target::LengthBias:        return "Length Bias";
        case Target::NoteProbabilityBias: return "Note P. Bias";

        case Target::RunMode:           return "Run Mode";
        case Target::FirstStep:         return "First Step";
        case Target::LastStep:          return "Last Step";

        case Target::Last:              break;
        }
        return nullptr;
    }

    static bool isEngineTarget(Target target) {
        return target >= Target::EngineFirst && target <= Target::EngineLast;
    }

    static bool isProjectTarget(Target target) {
        return target >= Target::ProjectFirst && target <= Target::ProjectLast;
    }

    static bool isTrackTarget(Target target) {
        return target >= Target::TrackFirst && target <= Target::TrackLast;
    }

    static bool isSequenceTarget(Target target) {
        return target >= Target::SequenceFirst && target <= Target::SequenceLast;
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

        void write(WriteContext &context) const;
        void read(ReadContext &context);

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

        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

        bool operator==(const MidiSource &other) const;

    private:
        MidiSourceConfig _source;
        Event _event;
        uint8_t _controlNumberOrNote;
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

        uint8_t tracks() const { return _tracks; }
        void setTracks(uint8_t tracks) {
            _tracks = tracks;
        }

        void toggleTrack(int trackIndex) {
            if (tracks() & (1<<trackIndex)) {
                setTracks(tracks() & ~(1<<trackIndex));
            } else {
                setTracks(tracks() | (1<<trackIndex));
            }
        }

        void printTracks(StringBuilder &str) const {
            if (isTrackTarget(_target) || isSequenceTarget(_target)) {
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
            setMin(min() + value * targetValueStep(_target));
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
            setMax(max() + value * targetValueStep(_target));
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

        void write(WriteContext &context) const;
        void read(ReadContext &context);

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

    void setRouted(Target target, uint8_t tracks, uint16_t patterns, bool routed);
    void writeTarget(Target target, uint8_t tracks, uint16_t patterns, float normalized);

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    bool isDirty() const { return _dirty; }
    void clearDirty() { _dirty = false; }

private:
    static float normalizeTargetValue(Target target, float value);
    static float denormalizeTargetValue(Target target, float normalized);
    static std::pair<float, float> normalizedDefaultRange(Target target);
    static float targetValueStep(Target target);
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

// Routable set.
template<Routing::Target First, Routing::Target Last>
struct RoutableSet {
    inline void clear() {
        _set.reset();
    }

    inline bool has(Routing::Target target) const {
        return _set.test(int(target) - int(First));
    }

    inline void set(Routing::Target target, bool routed) {
        _set.set(int(target) - int(First), routed);
    }

    inline void print(StringBuilder &str, Routing::Target target) const {
        if (has(target)) {
            str("\x1a");
        }
    }
private:
    std::bitset<int(Last) - int(First) + 1> _set;
};
