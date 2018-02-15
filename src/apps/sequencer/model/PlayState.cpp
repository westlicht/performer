#include "PlayState.h"


void PlayState::muteTrack(int track, ExecuteType executeType) {
    auto &trackState = _trackStates[track];
    trackState.setRequests(executeType == Immediate ? TrackState::ImmediateMuteRequest : TrackState::ScheduledMuteRequest);
    trackState.setRequestedMute(true);
    _dirty = true;
}

void PlayState::unmuteTrack(int track, ExecuteType executeType) {
    auto &trackState = _trackStates[track];
    trackState.setRequests(executeType == Immediate ? TrackState::ImmediateMuteRequest : TrackState::ScheduledMuteRequest);
    trackState.setRequestedMute(false);
    _dirty = true;
}

void PlayState::muteAll(ExecuteType executeType) {
    for (int track = 0; track < CONFIG_TRACK_COUNT; ++track) {
        muteTrack(track, executeType);
    }
}

void PlayState::unmuteAll(ExecuteType executeType) {
    for (int track = 0; track < CONFIG_TRACK_COUNT; ++track) {
        unmuteTrack(track, executeType);
    }
}

void PlayState::soloTrack(int track, ExecuteType executeType) {
    for (int otherTrack = 0; otherTrack < CONFIG_TRACK_COUNT; ++otherTrack) {
        if (track != otherTrack) {
            muteTrack(otherTrack, executeType);
        }
    }
}

void PlayState::unsoloTrack(int track, ExecuteType executeType) {
    for (int otherTrack = 0; otherTrack < CONFIG_TRACK_COUNT; ++otherTrack) {
        if (track != otherTrack) {
            unmuteTrack(otherTrack, executeType);
        }
    }
}

void PlayState::cancelScheduledMutesAndSolos() {
    for (int track = 0; track < CONFIG_TRACK_COUNT; ++track) {
        auto &trackState = _trackStates[track];
        trackState.clearRequests(TrackState::MuteRequests);
        trackState.setRequestedMute(trackState.mute());
    }
    _dirty = true;
}

void PlayState::fillTrack(int track, bool fill) {
    _trackStates[track].setFill(fill);
}

void PlayState::fillAll(bool fill) {
    for (int track = 0; track < CONFIG_TRACK_COUNT; ++track) {
        fillTrack(track, fill);
    }
}

void PlayState::selectTrackPattern(int track, int pattern, ExecuteType executeType) {
    auto &trackState = _trackStates[track];
    trackState.setRequests(executeType == Immediate ? TrackState::ImmediatePatternRequest : TrackState::ScheduledPatternRequest);
    trackState.setRequestedPattern(pattern);
    _dirty = true;
}

void PlayState::selectPattern(int pattern, ExecuteType executeType) {
    for (int track = 0; track < CONFIG_TRACK_COUNT; ++track) {
        selectTrackPattern(track, executeType);
    }
}
