#pragma once

#include "Config.h"

#include "Serialize.h"

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
        BPM,
        Swing,

        SequenceParams,

        FirstStep,
        LastStep,

        Last,
    };

    static const char *paramName(Param param) {
        switch (param) {
        case Param::BPM:            return "BPM";
        case Param::Swing:          return "Swing";
        case Param::FirstStep:      return "First Step";
        case Param::LastStep:       return "Last Step";
        case Param::SequenceParams: break;
        case Param::Last:           break;
        }
        return nullptr;
    }

    class CVSource {
    public:
        int index() const { return _index; }
        void setIndex(int index) { _index = index; }

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        uint8_t _index;
    };

    class TrackSource {
    public:
        int index() const { return _index; }
        void setIndex(int index) { _index = index; }

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        uint8_t _index;
    };

    class MIDISource {
    public:
        enum class Port : uint8_t {
            MIDI,
            USBMIDI,
        };

        enum class Kind : uint8_t {
            ControllerAbs,
            ControllerRel,
            PitchBend,
            NoteMomentary,
            NoteToggle,
            NoteVelocity,
        };

        Port port() const { return _port; }
        void setPort(Port port) { _port = port; }

        int channel() const { return _channel; }
        void setChannel(int channel) { _channel = channel; }

        Kind kind() const { return _kind; }
        void setKind(Kind kind) { _kind = kind; }

        int note() const { return _data.note.note; }
        void setNote(int note) { _data.note.note  = note; }

        int controller() const { return _data.cc.controller; }
        void setController(int controller) { _data.cc.controller = controller; }

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        Port _port;
        uint8_t _channel;
        Kind _kind;
        union {
            struct {
                uint8_t controller;
            } cc;
            struct {
                uint8_t note;
            } note;
        } _data;
    };

    class Source {
    public:
        enum class Kind : uint8_t {
            None,
            CV,
            Track,
            MIDI,
            Last,
        };

        // kind

        Kind kind() const { return _kind; }

        // cv

        const CVSource &cv() const { return _source.cv; }

        // track

        const TrackSource &track() const { return _source.track; }

        // midi

        const MIDISource &midi() const { return _source.midi; }
              MIDISource &midi()       { return _source.midi; }

        void clear();

        void initCV(int index);
        void initTrack(int index);
        void initMIDI();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        Kind _kind;
        union {
            CVSource cv;
            TrackSource track;
            MIDISource midi;
        } _source;
    };

    class Sink {
    public:

        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
    };

    class Route {
    public:
        // active

        bool active() const { return _active; }

        // param

        Param param() const { return _param; }

        // track

        int track() const { return _track; }

        // source

        const Source &source() const { return _source; }
              Source &source()       { return _source; }

        bool hasSource() const { return _source.kind() != Source::Kind::None; }

        void clear();

        void init(Param param, int track = -1);

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        bool _active;
        Param _param;
        int8_t _track;
        Source _source;

        float _shadowValue;

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

    Route *nextFreeRoute();
    const Route *findRoute(Param param, int trackIndex = -1) const;
          Route *findRoute(Param param, int trackIndex = -1);
    bool hasRoute(Param param, int trackIndex = -1) const { return findRoute(param, trackIndex) != nullptr; }

    Route *addRoute(Param param, int trackIndex = -1);
    void removeRoute(Route *route);

    void writeParam(Param param, int trackIndex, int patternIndex, float value);
    void writeTrackParam(Param param, int trackIndex, int patternIndex, float value);
    void writeNoteSequenceParam(NoteSequence &sequence, Param param, float value);
    void writeCurveSequenceParam(CurveSequence &sequence, Param param, float value);
    float readParam(Param param, int trackIndex, int patternIndex) const;

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    bool isDirty() const { return _dirty; }
    void clearDirty() { _dirty = false; }

private:
    Project &_project;
    RouteArray _routes;
    bool _dirty;
};
