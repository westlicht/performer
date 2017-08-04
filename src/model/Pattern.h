#pragma once

#include "Config.h"
#include "Sequence.h"

class Pattern {
public:
    typedef std::array<Sequence, TRACK_COUNT> SequenceArray;

    Pattern() {
        for (size_t i = 0; i < _sequences.size(); ++i) {
            _sequences[i].setPlayMode(Sequence::PlayMode(i % (Sequence::Random + 1)));
            // _sequences[i].setFirstStep(i);
            // _sequences[i].setLastStep(i + 4);
        }
    }

    const SequenceArray &sequences() const { return _sequences; }
          SequenceArray &sequences()       { return _sequences; }

    const Sequence &sequence(int index) const { return _sequences[index]; }
          Sequence &sequence(int index)       { return _sequences[index]; }

private:
    SequenceArray _sequences;
};
