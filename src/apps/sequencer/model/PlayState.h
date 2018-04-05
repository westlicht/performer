#pragma once

#include "Config.h"

#include "Serialize.h"

#include <array>

#include <cstdint>

class PlayState {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    enum ExecuteType {
        Immediate,
        Synced,
        Latched,
    };

    class TrackState {
    public:
        bool mute() const { return _state & Mute; }
        bool requestedMute() const { return _state & RequestedMute; }

        bool fill() const { return _state & Fill; }

        int pattern() const { return _pattern; }
        int requestedPattern() const { return _requestedPattern; }

        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        enum State {
            Mute                    = 1<<0,
            RequestedMute           = 1<<1,
            Fill                    = 1<<2,

            ImmediateMuteRequest    = 1<<3,
            SyncedMuteRequest       = 1<<4,
            LatchedMuteRequest      = 1<<5,

            ImmediatePatternRequest = 1<<6,
            SyncedPatternRequest    = 1<<7,
            LatchedPatternRequest   = 1<<8,

            MuteRequests = ImmediateMuteRequest | SyncedMuteRequest | LatchedMuteRequest,
            PatternRequests = ImmediatePatternRequest | SyncedPatternRequest | LatchedPatternRequest,
            ImmediateRequests = ImmediateMuteRequest | ImmediatePatternRequest,
            SyncedRequests = SyncedMuteRequest | SyncedPatternRequest,
            LatchedRequests = LatchedMuteRequest | LatchedPatternRequest
        };

        static State muteRequestFromExecuteType(ExecuteType type) {
            return State(1<<(3 + type));
        }

        static State patternRequestFromExecuteType(ExecuteType type) {
            return State(1<<(6 + type));
        }

        void setRequests(int requests) {
            _state |= uint16_t(requests);
        }

        void clearRequests(int requests) {
            _state &= ~uint16_t(requests);
        }

        bool hasRequests(int requests) const {
            return _state & uint16_t(requests);
        }

        void setMute(bool mute) {
            if (mute) {
                _state |= Mute;
            } else {
                _state &= ~Mute;
            }
        }

        void setRequestedMute(bool mute) {
            if (mute) {
                _state |= RequestedMute;
            } else {
                _state &= ~RequestedMute;
            }
        }

        void setFill(bool fill) {
            if (fill) {
                _state |= Fill;
            } else {
                _state &= ~Fill;
            }
        }

        void setPattern(int pattern) {
            _pattern = pattern;
        }

        void setRequestedPattern(int pattern) {
            _requestedPattern = pattern;
        }

        uint16_t _state;
        uint8_t _pattern;
        uint8_t _requestedPattern;

        friend class PlayState;
        friend class Engine;
    };

    //----------------------------------------
    // Properties
    //----------------------------------------

    // track states

    const TrackState &trackState(int track) const { return _trackStates[track]; }
          TrackState &trackState(int track)       { return _trackStates[track]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    // mutes

    void muteTrack(int track, ExecuteType executeType = Immediate);
    void unmuteTrack(int track, ExecuteType executeType = Immediate);
    void toggleMuteTrack(int track, ExecuteType executeType = Immediate);

    void muteAll(ExecuteType executeType = Immediate);
    void unmuteAll(ExecuteType executeType = Immediate);

    // solos

    void soloTrack(int track, ExecuteType executeType = Immediate);
    void unsoloTrack(int track, ExecuteType executeType = Immediate);

    // fills

    void fillTrack(int track, bool fill);
    void fillAll(bool fill);

    // pattern change

    void selectTrackPattern(int track, int pattern, ExecuteType executeType = Immediate);
    void selectPattern(int pattern, ExecuteType executeType = Immediate);


    void cancelMuteRequests();
    void cancelPatternRequests();
    void commitLatchedRequests() { _executeLatchedRequests = true; }

    bool hasImmediateRequests() const { return _hasImmediateRequests; }
    bool hasSyncedRequests() const { return _hasSyncedRequests; }
    bool hasLatchedRequests() const { return _hasLatchedRequests; }

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    void notify(ExecuteType executeType) {
        _hasImmediateRequests |= (executeType == Immediate);
        _hasSyncedRequests |= (executeType == Synced);
        _hasLatchedRequests |= (executeType == Latched);
    }

    bool executeLatchedRequests() const { return _executeLatchedRequests; }

    void clearImmediateRequests() { _hasImmediateRequests = false; }
    void clearSyncedRequests() { _hasSyncedRequests = false; }
    void clearLatchedRequests() { _hasLatchedRequests = false; _executeLatchedRequests = false; }

    std::array<TrackState, CONFIG_TRACK_COUNT> _trackStates;
    bool _executeLatchedRequests;
    bool _hasImmediateRequests;
    bool _hasSyncedRequests;
    bool _hasLatchedRequests;

    friend class Engine;
};
