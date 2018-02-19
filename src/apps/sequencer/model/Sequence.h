#pragma once

#include "Config.h"
#include "Serialize.h"
#include "NoteSequence.h"
#include "CurveSequence.h"
#include "TrackSetup.h"

#include <array>
#include <cstdint>
#include <initializer_list>

class Sequence {
public:
    //----------------------------------------
    // Properties
    //----------------------------------------

    // sequence

    const NoteSequence &noteSequence() const { return _sequence.noteSequence; }
          NoteSequence &noteSequence()       { return _sequence.noteSequence; }

    const CurveSequence &curveSequence() const { return _sequence.curveSequence; }
          CurveSequence &curveSequence()       { return _sequence.curveSequence; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear(TrackSetup::Mode mode);

    void write(WriteContext &context, int index) const;
    void read(ReadContext &context, int index);

private:
    union {
        NoteSequence noteSequence;
        CurveSequence curveSequence;
    } _sequence;
};
