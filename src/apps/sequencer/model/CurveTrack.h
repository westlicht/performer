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

    // FillMode

    enum class FillMode : uint8_t {
        None,
        Variation,
        NextPattern,
        Invert,
        Last
    };

    static const char *fillModeName(FillMode fillMode) {
        switch (fillMode) {
        case FillMode::None:        return "None";
        case FillMode::Variation:   return "Variation";
        case FillMode::NextPattern: return "Next Pattern";
        case FillMode::Invert:      return "Invert";
        case FillMode::Last:        break;
        }
        return nullptr;
    }

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

    FillMode fillMode() const { return _fillMode; }
    void setFillMode(FillMode fillMode) {
        _fillMode = ModelUtils::clampedEnum(fillMode);
    }

    void editFillMode(int value, bool shift) {
        setFillMode(ModelUtils::adjustedEnum(fillMode(), value));
    }

    void printFillMode(StringBuilder &str) const {
        str(fillModeName(fillMode()));
    }

    // slideTime

    int slideTime() const { return _slideTime.get(isRouted(Routing::Target::SlideTime)); }
    void setSlideTime(int slideTime, bool routed = false) {
        _slideTime.set(clamp(slideTime, 0, 100), routed);
    }

    void editSlideTime(int value, bool shift) {
        if (!isRouted(Routing::Target::SlideTime)) {
            setSlideTime(ModelUtils::adjustedByStep(slideTime(), value, 5, !shift));
        }
    }

    void printSlideTime(StringBuilder &str) const {
        printRouted(str, Routing::Target::SlideTime);
        str("%d%%", slideTime());
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
        printRouted(str, Routing::Target::Rotate);
        str("%+d", rotate());
    }

    // shapeProbabilityBias

    int shapeProbabilityBias() const { return _shapeProbabilityBias.get(isRouted(Routing::Target::ShapeProbabilityBias)); }
    void setShapeProbabilityBias(int shapeProbabilityBias, bool routed = false) {
        _shapeProbabilityBias.set(clamp(shapeProbabilityBias, -8, 8), routed);
    }

    void editShapeProbabilityBias(int value, bool shift) {
        if (!isRouted(Routing::Target::ShapeProbabilityBias)) {
            setShapeProbabilityBias(shapeProbabilityBias() + value);
        }
    }

    void printShapeProbabilityBias(StringBuilder &str) const {
        printRouted(str, Routing::Target::ShapeProbabilityBias);
        str("%+.1f%%", shapeProbabilityBias() * 12.5f);
    }

    // gateProbabilityBias

    int gateProbabilityBias() const { return _gateProbabilityBias.get(isRouted(Routing::Target::GateProbabilityBias)); }
    void setGateProbabilityBias(int gateProbabilityBias, bool routed = false) {
        _gateProbabilityBias.set(clamp(gateProbabilityBias, -CurveSequence::GateProbability::Range, CurveSequence::GateProbability::Range), routed);
    }

    void editGateProbabilityBias(int value, bool shift) {
        if (!isRouted(Routing::Target::GateProbabilityBias)) {
            setGateProbabilityBias(gateProbabilityBias() + value);
        }
    }

    void printGateProbabilityBias(StringBuilder &str) const {
        printRouted(str, Routing::Target::GateProbabilityBias);
        str("%+.1f%%", gateProbabilityBias() * 12.5f);
    }

    // sequences

    const CurveSequenceArray &sequences() const { return _sequences; }
          CurveSequenceArray &sequences()       { return _sequences; }

    const CurveSequence &sequence(int index) const { return _sequences[index]; }
          CurveSequence &sequence(int index)       { return _sequences[index]; }

    //----------------------------------------
    // Routing
    //----------------------------------------

    inline bool isRouted(Routing::Target target) const { return Routing::isRouted(target, _trackIndex); }
    inline void printRouted(StringBuilder &str, Routing::Target target) const { Routing::printRouted(str, target, _trackIndex); }
    void writeRouted(Routing::Target target, int intValue, float floatValue);

    //----------------------------------------
    // Methods
    //----------------------------------------

    CurveTrack() { clear(); }

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    void setTrackIndex(int trackIndex) {
        _trackIndex = trackIndex;
        for (auto &sequence : _sequences) {
            sequence.setTrackIndex(trackIndex);
        }
    }

    int8_t _trackIndex = -1;
    Types::PlayMode _playMode;
    FillMode _fillMode;
    Routable<uint8_t> _slideTime;
    Routable<int8_t> _rotate;
    Routable<int8_t> _shapeProbabilityBias;
    Routable<int8_t> _gateProbabilityBias;

    CurveSequenceArray _sequences;

    friend class Track;
};
