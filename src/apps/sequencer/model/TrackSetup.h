#pragma once

#include "Serialize.h"

#include <cstdint>
#include <cstring>

class TrackSetup {
public:
    enum class Mode : uint8_t {
        Note,
        Curve,
        Last
    };

    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::Note:    return "Note";
        case Mode::Curve:   return "Curve";
        case Mode::Last:    break;
        }
        return nullptr;
    }

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
        Last
    };

    static const char *fillModeName(FillMode fillMode) {
        switch (fillMode) {
        case FillMode::None:    return "None";
        case FillMode::Last:    break;
        }
        return nullptr;
    }

    // mode

    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

    // playMode

    PlayMode playMode() const { return _playMode; }
    void setPlayMode(PlayMode playMode) { _playMode = playMode; }

    // fillMode

    FillMode fillMode() const { return _fillMode; }
    void setFillMode(FillMode fillMode) { _fillMode = fillMode; }

    // linkTrack

    int linkTrack() const { return _linkTrack; }
    void setLinkTrack(int linkTrack) { _linkTrack = linkTrack; }

    // Serialization

    void write(WriteContext &context, int index) const;
    void read(ReadContext &context, int index);

    bool operator==(const TrackSetup &other) const {
        return std::memcmp(this, &other, sizeof(TrackSetup)) == 0;
    }
    bool operator!=(const TrackSetup &other) const {
        return !(*this == other);
    }

private:
    Mode _mode = Mode::Note;
    PlayMode _playMode = PlayMode::Loose;
    FillMode _fillMode = FillMode::None;
    int8_t _linkTrack = -1;
};
