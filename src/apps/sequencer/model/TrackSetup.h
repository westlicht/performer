#pragma once

#include "Config.h"
#include "Types.h"
#include "Serialize.h"
#include "ModelUtils.h"

#include "core/math/Math.h"
#include "core/utils/StringUtils.h"

#include <cstdint>
#include <cstring>

class TrackSetup {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    enum class PlayMode : uint8_t {
        Free,
        Aligned,
        Last
    };

    static const char *playModeName(PlayMode playMode) {
        switch (playMode) {
        case PlayMode::Free:    return "Free";
        case PlayMode::Aligned: return "Aligned";
        case PlayMode::Last:    break;
        }
        return nullptr;
    }

    enum class FillMode : uint8_t {
        None,
        // all gates
        // max gate probability
        Last
    };

    static const char *fillModeName(FillMode fillMode) {
        switch (fillMode) {
        case FillMode::None:    return "None";
        case FillMode::Last:    break;
        }
        return nullptr;
    }

    //----------------------------------------
    // Properties
    //----------------------------------------

    // trackIndex

    int trackIndex() const { return _trackIndex; }
    void setTrackIndex(int trackIndex) { _trackIndex = trackIndex; }

    // trackMode

    Types::TrackMode trackMode() const { return _trackMode; }
    void setTrackMode(Types::TrackMode trackMode) { _trackMode = trackMode; }

    void editTrackMode(int value, bool shift) {
        setTrackMode(ModelUtils::adjustedEnum(trackMode(), value));
    }

    void printTrackMode(StringBuilder &str) const {
        str(Types::trackModeName(trackMode()));
    }

    // playMode

    PlayMode playMode() const { return _playMode; }
    void setPlayMode(PlayMode playMode) { _playMode = playMode; }

    void editPlayMode(int value, bool shift) {
        setPlayMode(ModelUtils::adjustedEnum(playMode(), value));
    }

    void printPlayMode(StringBuilder &str) const {
        str(playModeName(playMode()));
    }

    // fillMode

    FillMode fillMode() const { return _fillMode; }
    void setFillMode(FillMode fillMode) { _fillMode = fillMode; }

    void editFillMode(int value, bool shift) {
        setFillMode(ModelUtils::adjustedEnum(fillMode(), value));
    }

    void printFillMode(StringBuilder &str) const {
        str(fillModeName(fillMode()));
    }

    // linkTrack

    int linkTrack() const { return _linkTrack; }
    void setLinkTrack(int linkTrack) {
        _linkTrack = clamp(linkTrack, -1, _trackIndex - 1);
    }

    void editLinkTrack(int value, bool shift) {
        setLinkTrack(linkTrack() + value);
    }

    void printLinkTrack(StringBuilder &str) const {
        if (linkTrack() == -1) {
            str("None");
        } else {
            str("Track%d", linkTrack() + 1);
        }
    }

    //----------------------------------------
    // Methods
    //----------------------------------------

    TrackSetup() { clear(); }

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    TrackSetup &operator=(const TrackSetup &other) {
        _trackMode = other._trackMode;
        _playMode = other._playMode;
        _fillMode = other._fillMode;
        _linkTrack = other._linkTrack;
        return *this;
    }

    bool operator==(const TrackSetup &other) const {
        return (
            _trackMode == other._trackMode &&
            _playMode == other._playMode &&
            _fillMode == other._fillMode &&
            _linkTrack == other._linkTrack
        );
    }

    bool operator!=(const TrackSetup &other) const {
        return !(*this == other);
    }

private:
    uint8_t _trackIndex = -1;
    Types::TrackMode _trackMode;
    PlayMode _playMode;
    FillMode _fillMode;
    int8_t _linkTrack;
};
