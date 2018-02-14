#pragma once

#include "Config.h"
#include "ClockSetup.h"
#include "Track.h"
#include "Pattern.h"
#include "Serialize.h"

#include "core/math/Math.h"

class Project {
public:
    // Parameters

    // bpm

    float bpm() const { return _bpm; }
    void setBpm(float bpm) { _bpm = clamp(bpm, 1.f, 1000.f); }

    // swing

    uint8_t swing() const { return _swing; }
    void setSwing(uint8_t swing) { _swing = swing; }

    // clockSetup

    const ClockSetup &clockSetup() const { return _clockSetup; }
          ClockSetup &clockSetup()       { return _clockSetup; }

    // Tracks

    typedef std::array<Track, CONFIG_TRACK_COUNT> TrackArray;

    const TrackArray &tracks() const { return _tracks; }
          TrackArray &tracks()       { return _tracks; }

    const Track &track(int index) const { return _tracks[index]; }
          Track &track(int index)       { return _tracks[index]; }

    const Track &selectedTrack() const { return _tracks[_selectedTrackIndex]; }
          Track &selectedTrack()       { return _tracks[_selectedTrackIndex]; }

    // Patterns

    typedef std::array<Pattern, CONFIG_PATTERN_COUNT> PatternArray;

    const PatternArray &patterns() const { return _patterns; }
          PatternArray &patterns()       { return _patterns; }

    const Pattern &pattern(int index) const { return _patterns[index]; }
          Pattern &pattern(int index)       { return _patterns[index]; }

    // Track selection

    int selectedTrackIndex() const { return _selectedTrackIndex; }
    void setSelectedTrackIndex(int index) { _selectedTrackIndex = index; }

    bool isSelectedTrack(int index) const { return _selectedTrackIndex == index; }

    const Sequence &selectedSequence() const { return selectedPattern().sequence(_selectedTrackIndex); }
          Sequence &selectedSequence()       { return selectedPattern().sequence(_selectedTrackIndex); }

    // Pattern selection

    int selectedPatternIndex() const { return _selectedPatternIndex; }
    void setSelectedPatternIndex(int index) { _selectedPatternIndex = index; }

    bool isSelectedPattern(int index) const { return _selectedPatternIndex == index; }

    const Pattern &selectedPattern() const { return _patterns[_selectedPatternIndex]; }
          Pattern &selectedPattern()       { return _patterns[_selectedPatternIndex]; }

    // helpers

    void setTrack(int trackIndex, const Track &track) {
        bool modeChanged = track.mode() != _tracks[trackIndex].mode();
        _tracks[trackIndex] = track;
        if (modeChanged) {
            // TODO reset snapshots
            for (auto &pattern : _patterns) {
                auto &sequence = pattern.sequence(trackIndex);
                switch (track.mode()) {
                case Track::Mode::Note:     sequence.noteSequence().setDefault(); break;
                case Track::Mode::Curve:    sequence.curveSequence().setDefault(); break;
                case Track::Mode::Last:     break;
                }
            }
        }
    }

    // Serialization

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    float _bpm = 120.f;
    uint8_t _swing = 0;

    ClockSetup _clockSetup;

    TrackArray _tracks;
    PatternArray _patterns;

    int _selectedTrackIndex = 0;
    int _selectedPatternIndex = 0;
};
