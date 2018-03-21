#pragma once

#include "Config.h"

#include "Track.h"
#include "Sequence.h"
#include "Project.h"

class TrackBuffer {
public:
    void clear() {
        _copied = false;
    }

    const Track &track() const { return _track; }

    bool isCopied() const { return _copied; }

    void copyFrom(const Track &track) {
        _track = track;
        _copied = true;
    }

    void pasteTo(Track &track) const {
        if (canPasteTo(track)) {
            track = _track;
        }
    }

    bool canPasteTo(Track &track) const {
        return true;
    }

private:
    Track _track;
    bool _copied;
};

class SequenceBuffer {
public:
    void clear() {
        _copied = false;
    }

    const Sequence &sequence() const { return _sequence; }

    bool isCopied() const { return _copied; }

    void copyFrom(const Sequence &sequence) {
        _sequence = sequence;
        _copied = true;
    }

    void pasteTo(Sequence &sequence) const {
        if (canPasteTo(sequence)) {
            sequence = _sequence;
        }
    }

    bool canPasteTo(Sequence &sequence) const {
        return _sequence.trackMode() == sequence.trackMode();
    }

private:
    Sequence _sequence;
    bool _copied;
};

class PatternBuffer {
public:
    void clear() {
        _copied = false;
    }

    const Sequence &sequence(int trackIndex) const { return _sequences[trackIndex]; }

    bool isCopied() const { return _copied; }

    void copyFrom(const Project &project, int patternIndex) {
        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            _sequences[trackIndex] = project.sequence(trackIndex, patternIndex);
        }
        _copied = true;
    }

    void pasteTo(Project &project, int patternIndex) const {
        if (canPasteTo(project, patternIndex)) {
            for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
                project.sequence(trackIndex, patternIndex) = _sequences[trackIndex];
            }
        }
    }

    bool canPasteTo(Project &project, int patternIndex) const {
        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            if (_sequences[trackIndex].trackMode() != project.sequence(trackIndex, patternIndex).trackMode()) {
                return false;
            }
        }
        return true;
    }

private:
    std::array<Sequence, CONFIG_TRACK_COUNT> _sequences;
    bool _copied;
};

class ClipBoard {
public:
    ClipBoard();

    void clear();

    TrackBuffer &trackBuffer() { return _trackBuffer; }
    SequenceBuffer &sequenceBuffer() { return _sequenceBuffer; }
    PatternBuffer &patternBuffer() { return _patternBuffer; }

private:
    TrackBuffer _trackBuffer;
    SequenceBuffer _sequenceBuffer;
    PatternBuffer _patternBuffer;
};
