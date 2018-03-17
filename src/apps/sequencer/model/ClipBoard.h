#pragma once

#include "TrackSetup.h"
#include "Sequence.h"

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

// class PatternBuffer {
// public:
//     void clear() {
//         _copied = false;
//     }

//     const Pattern &pattern() const { return _pattern; }

//     bool isCopied() const { return _copied; }

//     void copyFrom(const Pattern &pattern) {
//         _pattern = pattern;
//         _copied = true;
//     }

//     void pasteTo(Pattern &pattern) const {
//         if (canPasteTo(pattern)) {
//             pattern = _pattern;
//         }
//     }

//     bool canPasteTo(Pattern &pattern) const {
//         return true;
//     }

// private:
//     Pattern _pattern;
//     bool _copied;
// };

class ClipBoard {
public:
    ClipBoard();

    void clear();

    TrackSetupBuffer &trackSetupBuffer() { return _trackSetupBuffer; }
    SequenceBuffer &sequenceBuffer() { return _sequenceBuffer; }
    // PatternBuffer &patternBuffer() { return _patternBuffer; }

private:
    TrackSetupBuffer _trackSetupBuffer;
    SequenceBuffer _sequenceBuffer;
    // PatternBuffer _patternBuffer;
};
