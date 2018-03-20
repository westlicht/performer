#pragma once

#include "Config.h"

#include "Serialize.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>

#include <cstdint>

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
            CCAbsolute,
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
        // param

        Param param() const { return _param; }

        // track

        int track() const { return _track; }

        void clear();

        void init(Param param);
        void initTrack(Param param, int track);

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        Param _param;
        uint8_t _track;
    };

    class Route {
    public:
        const Source &source() const { return _source; }
              Source &source()       { return _source; }

        const Sink &sink() const { return _sink; }
              Sink &sink()       { return _sink; }

        bool active() const { return _active; }
        void enable() { _active = true; }

        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        bool _active;

        Source _source;
        Sink _sink;
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

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    bool isDirty() const { return _dirty; }
    void clearDirty() { _dirty = false; }

private:
    RouteArray _routes;
    bool _dirty;
};
