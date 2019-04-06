#pragma once

#include "Config.h"
#include "Types.h"
#include "CurveSequence.h"
#include "Serialize.h"
#include "Routing.h"

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

    int rotate() const { return _rotate.get(isRouted(Routing::Target::Rotate)); }
    void setRotate(int rotate, bool routed = false) {
        _rotate.set(clamp(rotate, -64, 64), routed);
    }

    void editRotate(int value, bool shift) {
        if (!isRouted(Routing::Target::Rotate)) {
            setRotate(rotate() + value);
        }
    }

    void printRotate(StringBuilder &str) const {
        _routed.print(str, Routing::Target::Rotate);
        str("%+d", rotate());
    }

    // sequences

    const CurveSequenceArray &sequences() const { return _sequences; }
          CurveSequenceArray &sequences()       { return _sequences; }

    const CurveSequence &sequence(int index) const { return _sequences[index]; }
          CurveSequence &sequence(int index)       { return _sequences[index]; }

    //----------------------------------------
    // Routing
    //----------------------------------------

    inline bool isRouted(Routing::Target target) const { return _routed.has(target); }
    inline void setRouted(Routing::Target target, bool routed) { _routed.set(target, routed); }
    void writeRouted(Routing::Target target, int intValue, float floatValue);

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
    Routable<int8_t> _rotate;

    RoutableSet<Routing::Target::TrackFirst, Routing::Target::TrackLast> _routed;

    CurveSequenceArray _sequences;
};
