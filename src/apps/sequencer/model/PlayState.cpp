#include "PlayState.h"

void PlayState::TrackState::clear() {
    _state = 0;
    _pattern = 0;
    _requestedPattern = 0;
}

void PlayState::TrackState::write(WriteContext &context) const {
    auto &writer = context.writer;
    uint8_t muteValue = mute();
    writer.write(muteValue);
    writer.write(_pattern);
}

void PlayState::TrackState::read(ReadContext &context) {
    auto &reader = context.reader;
    uint8_t muteValue;
    reader.read(muteValue);
    setMute(muteValue);
    reader.read(_pattern);
}

void PlayState::muteTrack(int track, ExecuteType executeType) {
    auto &trackState = _trackStates[track];
    trackState.setRequests(executeType == Immediate ? TrackState::ImmediateMuteRequest : TrackState::ScheduledMuteRequest);
    trackState.setRequestedMute(true);
    notify(executeType);
}

void PlayState::unmuteTrack(int track, ExecuteType executeType) {
    auto &trackState = _trackStates[track];
    trackState.setRequests(executeType == Immediate ? TrackState::ImmediateMuteRequest : TrackState::ScheduledMuteRequest);
    trackState.setRequestedMute(false);
    notify(executeType);
}

void PlayState::toggleMuteTrack(int track, ExecuteType executeType) {
    auto &trackState = _trackStates[track];
    switch (executeType) {
    case Immediate:
        if (trackState.mute()) {
            unmuteTrack(track, Immediate);
        } else {
            muteTrack(track, Immediate);
        }
        break;
    case Scheduled:
        if (trackState.requestedMute() == trackState.mute()) {
            if (trackState.mute()) {
                unmuteTrack(track, Scheduled);
            } else {
                muteTrack(track, Scheduled);
            }
        } else {
            if (trackState.mute()) {
                muteTrack(track, Scheduled);
            } else {
                unmuteTrack(track, Scheduled);
            }
        }
        break;
    }
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
    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        if (track == trackIndex) {
            unmuteTrack(trackIndex, executeType);
        } else {
            muteTrack(trackIndex, executeType);
        }
    }
}

void PlayState::unsoloTrack(int track, ExecuteType executeType) {
    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        if (track != trackIndex) {
            unmuteTrack(trackIndex, executeType);
        }
    }
}

void PlayState::cancelScheduledMutesAndSolos() {
    for (int track = 0; track < CONFIG_TRACK_COUNT; ++track) {
        auto &trackState = _trackStates[track];
        trackState.clearRequests(TrackState::MuteRequests);
        trackState.setRequestedMute(trackState.mute());
    }
}

void PlayState::fillTrack(int track, bool fill) {
    _trackStates[track].setFill(fill);
    notify(Immediate);
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
    notify(executeType);
}

void PlayState::selectPattern(int pattern, ExecuteType executeType) {
    for (int track = 0; track < CONFIG_TRACK_COUNT; ++track) {
        selectTrackPattern(track, pattern, executeType);
    }
}

void PlayState::clear() {
    for (auto &trackState : _trackStates) {
        trackState.clear();
    }
    _hasImmediateRequests = false;
    _hasScheduledRequests = false;
}

void PlayState::write(WriteContext &context) const {
    writeArray(context, _trackStates);
}

void PlayState::read(ReadContext &context) {
    readArray(context, _trackStates);
    notify(Immediate);
}
