#pragma once

#include "Config.h"
#include "Sequence.h"
#include "Serialize.h"

class Pattern {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef std::array<Sequence, CONFIG_TRACK_COUNT> SequenceArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    const SequenceArray &sequences() const { return _sequences; }
          SequenceArray &sequences()       { return _sequences; }

    const Sequence &sequence(int index) const { return _sequences[index]; }
          Sequence &sequence(int index)       { return _sequences[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear();

    void write(WriteContext &context, int index) const;
    void read(ReadContext &context, int index);

private:
    SequenceArray _sequences;
};
