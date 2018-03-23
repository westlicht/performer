#pragma once

#include "Config.h"
#include "NoteSequence.h"
#include "Serialize.h"

class MidiCvTrack {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    //----------------------------------------
    // Properties
    //----------------------------------------

    //----------------------------------------
    // Methods
    //----------------------------------------

    MidiCvTrack() { clear(); }

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
};
