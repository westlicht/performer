#pragma once

#include "Types.h"
#include "Serialize.h"

#include <cstdint>
#include <cstring>

class TrackSetup {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    enum class PlayMode : uint8_t {
        Loose,
        Rigid,
        Last
    };

    static const char *playModeName(PlayMode playMode) {
        switch (playMode) {
        case PlayMode::Loose:   return "Loose";
        case PlayMode::Rigid:   return "Rigid";
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

    // trackMode

    Types::TrackMode trackMode() const { return _trackMode; }
    void setTrackMode(Types::TrackMode trackMode) { _trackMode = trackMode; }

    // playMode

    PlayMode playMode() const { return _playMode; }
    void setPlayMode(PlayMode playMode) { _playMode = playMode; }

    // fillMode

    FillMode fillMode() const { return _fillMode; }
    void setFillMode(FillMode fillMode) { _fillMode = fillMode; }

    // linkTrack

    int linkTrack() const { return _linkTrack; }
    void setLinkTrack(int linkTrack) { _linkTrack = linkTrack; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    bool operator==(const TrackSetup &other) const {
        return std::memcmp(this, &other, sizeof(TrackSetup)) == 0;
    }
    bool operator!=(const TrackSetup &other) const {
        return !(*this == other);
    }

private:
    Types::TrackMode _trackMode;
    PlayMode _playMode;
    FillMode _fillMode;
    int8_t _linkTrack;
};
