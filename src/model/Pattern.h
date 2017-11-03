#pragma once

#include "Config.h"
#include "Sequence.h"
#include "Serialize.h"

class Pattern {
public:
    typedef std::array<Sequence, CONFIG_TRACK_COUNT> SequenceArray;

    Pattern() {
        for (size_t i = 0; i < _sequences.size(); ++i) {
            // _sequences[i].setPlayMode(Sequence::PlayMode(i % (Sequence::Random + 1)));
            // _sequences[i].setFirstStep(i);
            // _sequences[i].setLastStep(i + 4);
        }

        _sequences[0].setGates({ 1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0 });
        _sequences[1].setGates({ 0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0 });
        _sequences[2].setGates({ 0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0 });
        _sequences[3].setGates({ 0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0 });
        _sequences[4].setGates({ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 });
        _sequences[5].setGates({ 0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0 });

        _sequences[7].setGates({ 0,0,1,0,0,0,1,0,0,0,1,0,0,1,1,0 });
        _sequences[7].setNotes({ 0,0,12,0,0,0,0,0,0,0,12,0,0,9,11,0 });
    }

    const SequenceArray &sequences() const { return _sequences; }
          SequenceArray &sequences()       { return _sequences; }

    const Sequence &sequence(int index) const { return _sequences[index]; }
          Sequence &sequence(int index)       { return _sequences[index]; }

    // Serialization

    void write(ModelWriter &writer) const {
        for (const auto &sequence : _sequences) {
            sequence.write(writer);
        }
    }

    void read(ModelReader &reader) {
        for (auto &sequence : _sequences) {
            sequence.read(reader);
        }
    }

private:
    SequenceArray _sequences;
};
