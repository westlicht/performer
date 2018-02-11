#pragma once

#include "Config.h"
#include "Sequence.h"
#include "Serialize.h"

class Pattern {
public:
    typedef std::array<Sequence, CONFIG_TRACK_COUNT> SequenceArray;

    Pattern() {
        _sequences[0].noteSequence().setGates({ 1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0 });
        _sequences[1].noteSequence().setGates({ 0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0 });
        _sequences[2].noteSequence().setGates({ 0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0 });
        _sequences[3].noteSequence().setGates({ 0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0 });
        _sequences[4].noteSequence().setGates({ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 });
        _sequences[5].noteSequence().setGates({ 0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0 });
        _sequences[7].noteSequence().setGates({ 1,0,1,0,1,0,1,1,1,1,1,0,1,1,0,1 });

        _sequences[7].noteSequence().setNotes({ 36,36,36,36,48,36,48,37,60,61,58,36,39,42,48,37 });

    #if 0
        _sequences[0].setGates({ 1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0 });
        _sequences[1].setGates({ 0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0 });
        _sequences[2].setGates({ 0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0 });
        _sequences[3].setGates({ 0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0 });
        _sequences[4].setGates({ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 });
        _sequences[5].setGates({ 0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0 });

        _sequences[7].setGates({ 1,0,1,0,1,0,1,1,1,1,1,0,1,1,0,1 });
        _sequences[7].setNotes({ 36,36,36,36,48,36,48,37,60,61,58,36,39,42,48,37 });
    #endif
    }

    const SequenceArray &sequences() const { return _sequences; }
          SequenceArray &sequences()       { return _sequences; }

    const Sequence &sequence(int index) const { return _sequences[index]; }
          Sequence &sequence(int index)       { return _sequences[index]; }

    // Serialization

    void write(WriteContext &context, int index) const;
    void read(ReadContext &context, int index);

private:
    SequenceArray _sequences;
};
