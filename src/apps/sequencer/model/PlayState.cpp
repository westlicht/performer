#include "PlayState.h"

#include "Project.h"
#include "ProjectVersion.h"

// PlayState::TrackState

void PlayState::TrackState::clear() {
    _state = 0;
    _pattern = 0;
    _requestedPattern = 0;
    _fillAmount = 100;
}

void PlayState::TrackState::write(WriteContext &context) const {
    auto &writer = context.writer;
    uint8_t muteValue = mute();
    writer.write(muteValue);
    // make sure to not write snapshot state
    uint8_t patternValue = _pattern < CONFIG_PATTERN_COUNT ? _pattern : 0;
    writer.write(patternValue);
    writer.write(_fillAmount);
}

void PlayState::TrackState::read(ReadContext &context) {
    auto &reader = context.reader;
    uint8_t muteValue;
    reader.read(muteValue);
    setMute(muteValue);
    reader.read(_pattern);
    reader.read(_fillAmount, ProjectVersion::Version12);
}

// PlayState::SongState

void PlayState::SongState::clear() {
    _state = 0;
}

// PlayState

PlayState::PlayState(Project &project) :
    _project(project)
{}

void PlayState::muteTrack(int track, ExecuteType executeType) {
    auto &trackState = _trackStates[track];
    trackState.setRequests(TrackState::muteRequestFromExecuteType(executeType));
    trackState.setRequestedMute(true);
    notify(executeType);
}

void PlayState::unmuteTrack(int track, ExecuteType executeType) {
    auto &trackState = _trackStates[track];
    trackState.setRequests(TrackState::muteRequestFromExecuteType(executeType));
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
    case Synced:
    case Latched:
        if (trackState.requestedMute() == trackState.mute()) {
            if (trackState.mute()) {
                unmuteTrack(track, executeType);
            } else {
                muteTrack(track, executeType);
            }
        } else {
            if (trackState.mute()) {
                muteTrack(track, executeType);
            } else {
                unmuteTrack(track, executeType);
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

void PlayState::fillTrack(int track, bool fill, bool hold) {
    _trackStates[track].setFill(fill, hold);
    notify(Immediate);
}

void PlayState::fillAll(bool fill, bool hold) {
    for (int track = 0; track < CONFIG_TRACK_COUNT; ++track) {
        fillTrack(track, fill, hold);
    }
}

void PlayState::selectTrackPattern(int track, int pattern, ExecuteType executeType) {
    if (_snapshot.active) {
        return;
}

    selectTrackPatternUnsafe(track, pattern, executeType);

    // switch selected pattern
    if (track == _project.selectedTrackIndex()) {
        _project.setSelectedPatternIndex(pattern);
    }
}

void PlayState::selectPattern(int pattern, ExecuteType executeType) {
    if (_snapshot.active) {
        return;
    }

    for (int track = 0; track < CONFIG_TRACK_COUNT; ++track) {
        selectTrackPattern(track, pattern, executeType);
    }
}

void PlayState::createSnapshot() {
    if (_snapshot.active) {
        return;
    }

    cancelPatternRequests();

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        int trackPatternIndex = trackState(trackIndex).pattern();
        _snapshot.lastTrackPatternIndex[trackIndex] = trackPatternIndex;
        _project.track(trackIndex).copyPattern(trackPatternIndex, SnapshotPatternIndex);
        selectTrackPattern(trackIndex, SnapshotPatternIndex);
    }

    _snapshot.lastSelectedPatternIndex = _project.selectedPatternIndex();
    _snapshot.active = true;
}

void PlayState::revertSnapshot(int targetPattern) {
    if (!_snapshot.active) {
        return;
    }

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        selectTrackPatternUnsafe(trackIndex, targetPattern >= 0 ? targetPattern : _snapshot.lastTrackPatternIndex[trackIndex]);
    }

    _project.setSelectedPatternIndex(targetPattern >= 0 ? targetPattern : _snapshot.lastSelectedPatternIndex);

    _snapshot.active = false;
}

void PlayState::commitSnapshot(int targetPattern) {
    if (!_snapshot.active) {
        return;
    }

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        int trackPatternIndex = targetPattern >= 0 ? targetPattern : _snapshot.lastTrackPatternIndex[trackIndex];
        _project.track(trackIndex).copyPattern(SnapshotPatternIndex, trackPatternIndex);
        selectTrackPatternUnsafe(trackIndex, trackPatternIndex);
    }

    _project.setSelectedPatternIndex(targetPattern >= 0 ? targetPattern : _snapshot.lastSelectedPatternIndex);

    _snapshot.active = false;
}

void PlayState::cancelMuteRequests() {
    for (int track = 0; track < CONFIG_TRACK_COUNT; ++track) {
        auto &trackState = _trackStates[track];
        trackState.clearRequests(TrackState::MuteRequests);
        trackState.setRequestedMute(trackState.mute());
    }
}

void PlayState::cancelPatternRequests() {
    for (int track = 0; track < CONFIG_TRACK_COUNT; ++track) {
        auto &trackState = _trackStates[track];
        trackState.clearRequests(TrackState::PatternRequests);
        trackState.setRequestedPattern(trackState.pattern());

        // switch selected pattern
        if (track == _project.selectedTrackIndex()) {
            _project.setSelectedPatternIndex(trackState.pattern());
        }
    }
}

void PlayState::playSong(int slot, ExecuteType executeType) {
    _songState.setRequestedSlot(slot);
    _songState.setRequests(SongState::playRequestFromExecuteType(executeType));
    notify(executeType);
}

void PlayState::stopSong(ExecuteType executeType) {
    _songState.setRequests(SongState::stopRequestFromExecuteType(executeType));
    notify(executeType);
}

void PlayState::clear() {
    for (auto &trackState : _trackStates) {
        trackState.clear();
    }

    _songState.clear();

    _executeLatchedRequests = false;
    _hasImmediateRequests = false;
    _hasSyncedRequests = false;
    _hasLatchedRequests = false;

    _snapshot.active = false;
}

void PlayState::write(WriteContext &context) const {
    writeArray(context, _trackStates);
}

void PlayState::read(ReadContext &context) {
    readArray(context, _trackStates);
    notify(Immediate);
}

void PlayState::selectTrackPatternUnsafe(int track, int pattern, ExecuteType executeType) {
    auto &trackState = _trackStates[track];
    trackState.setRequests(TrackState::patternRequestFromExecuteType(executeType));
    trackState.setRequestedPattern(pattern);
    notify(executeType);
}

void PlayState::writeRouted(Routing::Target target, uint8_t tracks, int intValue, float floatValue) {
    bool active = intValue != 0;

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        if (tracks & (1 << trackIndex)) {
            auto &trackState = this->trackState(trackIndex);
            switch (target) {
            case Routing::Target::Mute:
                if (trackState.mute() != active || trackState.requestedMute() != active) {
                    if (active) {
                        muteTrack(trackIndex);
                    } else {
                        unmuteTrack(trackIndex);
                    }
                }
                break;
            case Routing::Target::Fill:
                if (trackState.fill() != active) {
                    fillTrack(trackIndex, active);
                }
                break;
            case Routing::Target::FillAmount:
                trackState.setFillAmount(intValue);
                break;
            case Routing::Target::Pattern:
                if (trackState.pattern() != intValue || trackState.requestedPattern() != intValue) {
                    selectTrackPattern(trackIndex, intValue);
                }
                break;
            default:
                break;
            }
        }
    }
}
