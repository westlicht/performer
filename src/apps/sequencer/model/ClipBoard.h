#pragma once

#include "Config.h"

#include "TrackSetup.h"
#include "Sequence.h"
#include "Project.h"

class TrackSetupBuffer {
public:
    void clear() {
        _copied = false;
    }

    const TrackSetup &trackSetup() const { return _trackSetup; }

    bool isCopied() const { return _copied; }

    void copyFrom(const TrackSetup &trackSetup) {
        _trackSetup = trackSetup;
        _copied = true;
    }

    void pasteTo(TrackSetup &trackSetup) const {
        if (canPasteTo(trackSetup)) {
            trackSetup = _trackSetup;
        }
    }

    bool canPasteTo(TrackSetup &trackSetup) const {
        return true;
    }

private:
    TrackSetup _trackSetup;
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
        return true;
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

    TrackSetupBuffer &trackSetupBuffer() { return _trackSetupBuffer; }
    SequenceBuffer &sequenceBuffer() { return _sequenceBuffer; }
    PatternBuffer &patternBuffer() { return _patternBuffer; }

private:
    TrackSetupBuffer _trackSetupBuffer;
    SequenceBuffer _sequenceBuffer;
    PatternBuffer _patternBuffer;
};
