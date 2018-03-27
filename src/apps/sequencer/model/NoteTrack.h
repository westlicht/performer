#pragma once

#include "Config.h"
#include "Types.h"
#include "NoteSequence.h"
#include "Serialize.h"

class NoteTrack {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef std::array<NoteSequence, CONFIG_PATTERN_COUNT> NoteSequenceArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // playMode

    Types::PlayMode playMode() const { return _playMode; }
    void setPlayMode(Types::PlayMode playMode) { _playMode = playMode; }

    void editPlayMode(int value, bool shift) {
        setPlayMode(ModelUtils::adjustedEnum(playMode(), value));
    }

    void printPlayMode(StringBuilder &str) const {
        str(Types::playModeName(playMode()));
    }

    // fillMode

    Types::FillMode fillMode() const { return _fillMode; }
    void setFillMode(Types::FillMode fillMode) { _fillMode = fillMode; }

    void editFillMode(int value, bool shift) {
        setFillMode(ModelUtils::adjustedEnum(fillMode(), value));
    }

    void printFillMode(StringBuilder &str) const {
        str(Types::fillModeName(fillMode()));
    }

    // transpose

    int transpose() const { return _transpose; }
    void setTranspose(int transpose) { _transpose = clamp(transpose, -100, 100); }

    void editTranspose(int value, bool shift) {
        setTranspose(transpose() + value);
    }

    void printTranspose(StringBuilder &str) const {
        str("%+d", transpose());
    }

    // rotate

    int rotate() const { return _rotate; }
    void setRotate(int rotate) { _rotate = clamp(rotate, -64, 64); }

    void editRotate(int value, bool shift) {
        setRotate(rotate() + value);
    }

    void printRotate(StringBuilder &str) const {
        str("%+d", rotate());
    }

    // sequences

    const NoteSequenceArray &sequences() const { return _sequences; }
          NoteSequenceArray &sequences()       { return _sequences; }

    const NoteSequence &sequence(int index) const { return _sequences[index]; }
          NoteSequence &sequence(int index)       { return _sequences[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    NoteTrack() { clear(); }

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    Types::PlayMode _playMode;
    Types::FillMode _fillMode;
    NoteSequenceArray _sequences;
    int8_t _transpose;
    int8_t _rotate;
};
