#pragma once

#include "Config.h"
#include "Types.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "NoteTrack.h"
#include "CurveTrack.h"

#include "core/Debug.h"
#include "core/math/Math.h"
#include "core/utils/StringUtils.h"
#include "core/utils/Container.h"

#include <cstdint>
#include <cstring>

class Track {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    enum class TrackMode : uint8_t {
        Note,
        Curve,
        Last,
        Default = Note
    };

    static const char *trackModeName(TrackMode trackMode) {
        switch (trackMode) {
        case TrackMode::Note:    return "Note";
        case TrackMode::Curve:   return "Curve";
        case TrackMode::Last:    break;
        }
        return nullptr;
    }

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

    typedef std::array<NoteSequence, CONFIG_PATTERN_COUNT> NoteSequenceArray;
    typedef std::array<CurveSequence, CONFIG_PATTERN_COUNT> CurveSequenceArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // trackIndex

    int trackIndex() const { return _trackIndex; }
    void setTrackIndex(int trackIndex) { _trackIndex = trackIndex; }

    // trackMode

    TrackMode trackMode() const { return _trackMode; }
    void setTrackMode(TrackMode trackMode) {
        if (trackMode != _trackMode) {
            _trackMode = trackMode;
            setupTrack();
        }
    }

    void printTrackMode(StringBuilder &str) const {
        str(trackModeName(trackMode()));
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

    // noteTrack

    const NoteTrack &noteTrack() const { ASSERT(_trackMode == TrackMode::Note, "invalid mode"); return *_track.note; }
          NoteTrack &noteTrack()       { ASSERT(_trackMode == TrackMode::Note, "invalid mode"); return *_track.note; }

    // curveTrack

    const CurveTrack &curveTrack() const { ASSERT(_trackMode == TrackMode::Curve, "invalid mode"); return *_track.curve; }
          CurveTrack &curveTrack()       { ASSERT(_trackMode == TrackMode::Curve, "invalid mode"); return *_track.curve; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    Track() { clear(); }

    void clear();

    void clearPattern(int patternIndex);

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    Track &operator=(const Track &other) {
        setTrackMode(other._trackMode);
        _playMode = other._playMode;
        _fillMode = other._fillMode;
        _linkTrack = other._linkTrack;
        _container = other._container;
        return *this;
    }

private:
    void setupTrack();

    uint8_t _trackIndex = -1;
    TrackMode _trackMode;
    PlayMode _playMode;
    FillMode _fillMode;
    int8_t _linkTrack;

    Container<NoteTrack, CurveTrack> _container;
    union {
        NoteTrack *note;
        CurveTrack *curve;
    } _track;
};
