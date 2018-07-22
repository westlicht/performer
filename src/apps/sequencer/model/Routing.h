#pragma once

#include "Config.h"

#include "Types.h"
#include "Serialize.h"
#include "ModelUtils.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>

#include <cstdint>

class Project;
class NoteSequence;
class CurveSequence;

class Routing {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    enum class Param : uint8_t {
        None,
        // Global parameters
        Tempo,
        GlobalFirst = Tempo,
        Swing,
        GlobalLast = Swing,

        // Global modulations
        // GlobalModFirst,
        // CvOffset = GlobalModFirst,
        // GateProbabilityOffset,
        // GateLengthOffset,
        // GateLengthProbabilityOffset,
        // RetriggerOffset,
        // RetriggerProbabilityOffset,
        // SequenceShift,
        // GlobalModLast = SequenceShift,

        // Track parameters
        TrackSlideTime,
        TrackFirst = TrackSlideTime,
        TrackOctave,
        TrackTranspose,
        TrackRotate,
        TrackStepProbabilityBias,
        TrackStepLengthBias,
        TrackLast = TrackStepLengthBias,

        // Sequence parameters
        FirstStep,
        SequenceFirst = FirstStep,
        LastStep,
        SequenceLast = LastStep,

        Last,
    };

    static const char *paramName(Param param) {
        switch (param) {
        case Param::None:           return "None";
        case Param::Tempo:          return "Tempo";
        case Param::Swing:          return "Swing";

        case Param::TrackSlideTime: return "Slide Time";
        case Param::TrackOctave:    return "Octave";
        case Param::TrackTranspose: return "Transpose";
        case Param::TrackRotate:    return "Rotate";
        case Param::TrackStepProbabilityBias: return "Step P. Bias";
        case Param::TrackStepLengthBias: return "Step L. Bias";

        case Param::FirstStep:      return "First Step";
        case Param::LastStep:       return "Last Step";

        case Param::Last:           break;
        }
        return nullptr;
    }

    static bool isTrackParam(Param param) {
        return param >= Param::TrackFirst && param <= Param::TrackLast;
    }

    static bool isSequenceParam(Param param) {
        return param >= Param::SequenceFirst && param <= Param::SequenceLast;
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

        // port

        Types::MidiPort port() const { return _port; }
        void setPort(Types::MidiPort port) {
            _port = ModelUtils::clampedEnum(port);
        }

        void editPort(int value, bool shift) {
            setPort(ModelUtils::adjustedEnum(port(), value));
        }

        void printPort(StringBuilder &str) const {
            str(Types::midiPortName(port()));
        }

        // channel

        int channel() const { return _channel; }
        void setChannel(int channel) {
            _channel = clamp(channel, -1, 15);
        }

        void editChannel(int value, bool shift) {
            setChannel(channel() + value);
        }

        void printChannel(StringBuilder &str) const {
            Types::printMidiChannel(str, channel());
        }

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
        Types::MidiPort _port;
        int8_t _channel;
        Event _event;
        uint8_t _controlNumberOrNote;
    };

    class Route {
    public:
        // param

        Param param() const { return _param; }
        void setParam(Param param) {
            _param = ModelUtils::clampedEnum(param);
        }

        void editParam(int value, bool shift) {
            setParam(ModelUtils::adjustedEnum(param(), value));
        }

        void printParam(StringBuilder &str) const {
            str(paramName(param()));
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
            if (isTrackParam(_param)) {
                for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
                    str("%c", (_tracks & (1<<i)) ? 'X' : '-');
                }
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
            setMin(min() + value * paramValueStep(_param));
        }

        void printMin(StringBuilder &str) const {
            Routing::printParamValue(_param, _min, str);
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
            setMax(max() + value * paramValueStep(_param));
        }

        void printMax(StringBuilder &str) const {
            Routing::printParamValue(_param, _max, str);
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

        bool active() const { return _param != Param::None; }

        void init(Param param, int track = -1);

        void write(WriteContext &context) const;
        void read(ReadContext &context);

        bool operator==(const Route &other) const;
        bool operator!=(const Route &other) const {
            return !(*this == other);
        }

    private:
        Param _param;
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
    int findRoute(Param param, int trackIndex) const;

    void writeParam(Param param, int trackIndex, int patternIndex, float normalized);
    float readParam(Param param, int trackIndex, int patternIndex) const;

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    bool isDirty() const { return _dirty; }
    void clearDirty() { _dirty = false; }

private:
    void writeParam(Param param, int trackIndex, int patternIndex, float floatValue, int intValue);
    void writeTrackParam(Param param, int trackIndex, int patternIndex, float floatValue, int intValue);
    void writeNoteSequenceParam(NoteSequence &sequence, Param param, float floatValue, int intValue);
    void writeCurveSequenceParam(CurveSequence &sequence, Param param, float floatValue, int intValue);

    static float normalizeParamValue(Param param, float value);
    static float denormalizeParamValue(Param param, float normalized);
    static float paramValueStep(Param param);
    static void printParamValue(Param param, float normalized, StringBuilder &str);

    Project &_project;
    RouteArray _routes;
    bool _dirty;
};
