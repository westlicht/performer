#pragma once

#include "Config.h"
#include "Types.h"
#include "CurveSequence.h"
#include "Serialize.h"

class CurveTrack {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef std::array<CurveSequence, CONFIG_PATTERN_COUNT + CONFIG_SNAPSHOT_COUNT> CurveSequenceArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // playMode

    Types::PlayMode playMode() const { return _playMode; }
    void setPlayMode(Types::PlayMode playMode) {
        _playMode = ModelUtils::clampedEnum(playMode);
    }

    void editPlayMode(int value, bool shift) {
        setPlayMode(ModelUtils::adjustedEnum(playMode(), value));
    }

    void printPlayMode(StringBuilder &str) const {
        str(Types::playModeName(playMode()));
    }

    // fillMode

    Types::FillMode fillMode() const { return _fillMode; }
    void setFillMode(Types::FillMode fillMode) {
        _fillMode = ModelUtils::clampedEnum(fillMode);
    }

    void editFillMode(int value, bool shift) {
        setFillMode(ModelUtils::adjustedEnum(fillMode(), value));
    }

    void printFillMode(StringBuilder &str) const {
        str(Types::fillModeName(fillMode()));
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

    const CurveSequenceArray &sequences() const { return _sequences; }
          CurveSequenceArray &sequences()       { return _sequences; }

    const CurveSequence &sequence(int index) const { return _sequences[index]; }
          CurveSequence &sequence(int index)       { return _sequences[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    CurveTrack() { clear(); }

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    Types::PlayMode _playMode;
    Types::FillMode _fillMode;
    int8_t _rotate;
    CurveSequenceArray _sequences;
};
