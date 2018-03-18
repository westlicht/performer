#pragma once

#include "Config.h"
#include "Types.h"
#include "Serialize.h"
#include "NoteSequence.h"
#include "CurveSequence.h"

#include <array>
#include <cstdint>
#include <initializer_list>

class Sequence {
public:
    //----------------------------------------
    // Properties
    //----------------------------------------

    // trackMode

    Types::TrackMode trackMode() const { return _trackMode; }
    void setTrackMode(Types::TrackMode trackMode) {
        if (trackMode != _trackMode) {
            _trackMode = trackMode;
            clear();
        }
    }

    // sequence

    const NoteSequence &noteSequence() const { return _sequence.noteSequence; }
          NoteSequence &noteSequence()       { return _sequence.noteSequence; }

    const CurveSequence &curveSequence() const { return _sequence.curveSequence; }
          CurveSequence &curveSequence()       { return _sequence.curveSequence; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    Types::TrackMode _trackMode = Types::TrackMode::Default;

    union {
        NoteSequence noteSequence;
        CurveSequence curveSequence;
    } _sequence;
};
