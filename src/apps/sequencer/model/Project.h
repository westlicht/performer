#pragma once

#include "Config.h"
#include "ClockSetup.h"
#include "TrackSetup.h"
#include "Pattern.h"
#include "PlayState.h"
#include "Serialize.h"

#include "core/math/Math.h"
#include "core/utils/StringUtils.h"

class Project {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    static constexpr size_t NameLength = 16;

    typedef std::array<TrackSetup, CONFIG_TRACK_COUNT> TrackSetupArray;
    typedef std::array<Pattern, CONFIG_PATTERN_COUNT> PatternArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // slot

    int slot() const { return _slot; }
    void setSlot(int slot) {
        _slot = slot;
    }
    bool slotAssigned() const {
        return _slot != uint8_t(-1);
    }

    // name

    const char *name() const { return _name; }
    void setName(const char *name) {
        StringUtils::copy(_name, name, sizeof(_name));
    }

    // bpm

    float bpm() const { return _bpm; }
    void setBpm(float bpm) {
        _bpm = clamp(bpm, 1.f, 1000.f);
    }

    // swing

    int swing() const { return _swing; }
    void setSwing(int swing) {
        _swing = clamp(swing, 50, 99);
    }

    // globalMeasure

    int globalMeasure() const { return _globalMeasure; }
    void setGlobalMeasure(int globalMeasure) {
        _globalMeasure = clamp(globalMeasure, 1, 128);
    }

    // clockSetup

    const ClockSetup &clockSetup() const { return _clockSetup; }
          ClockSetup &clockSetup()       { return _clockSetup; }

    // trackSetups

    const TrackSetupArray &tracksSetups() const { return _trackSetups; }
          TrackSetupArray &tracksSetups()       { return _trackSetups; }

    const TrackSetup &trackSetup(int index) const { return _trackSetups[index]; }
          TrackSetup &trackSetup(int index)       { return _trackSetups[index]; }

    // playState

    const PlayState &playState() const { return _playState; }
          PlayState &playState()       { return _playState; }

    // patterns

    const PatternArray &patterns() const { return _patterns; }
          PatternArray &patterns()       { return _patterns; }

    const Pattern &pattern(int index) const { return _patterns[index]; }
          Pattern &pattern(int index)       { return _patterns[index]; }

    // selectedTrackIndex

    int selectedTrackIndex() const { return _selectedTrackIndex; }
    void setSelectedTrackIndex(int index) { _selectedTrackIndex = index; }
    bool isSelectedTrack(int index) const { return _selectedTrackIndex == index; }

    // selectedPatternIndex

    int selectedPatternIndex() const { return _selectedPatternIndex; }
    void setSelectedPatternIndex(int index) { _selectedPatternIndex = index; }
    bool isSelectedPattern(int index) const { return _selectedPatternIndex == index; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void setTrackSetup(int index, const TrackSetup &trackSetup);

    const TrackSetup &selectedTrackSetup() const { return _trackSetups[_selectedTrackIndex]; }
          TrackSetup &selectedTrackSetup()       { return _trackSetups[_selectedTrackIndex]; }

    const Sequence &selectedSequence() const { return selectedPattern().sequence(_selectedTrackIndex); }
          Sequence &selectedSequence()       { return selectedPattern().sequence(_selectedTrackIndex); }

    const Pattern &selectedPattern() const { return _patterns[_selectedPatternIndex]; }
          Pattern &selectedPattern()       { return _patterns[_selectedPatternIndex]; }

    const Sequence &selectedTrackSequence() const { return _patterns[_selectedPatternIndex].sequence(_selectedTrackIndex); }
          Sequence &selectedTrackSequence()       { return _patterns[_selectedPatternIndex].sequence(_selectedTrackIndex); }

    const Sequence &activeTrackSequence(int index) const { return _patterns[_selectedPatternIndex].sequence(index); }
          Sequence &activeTrackSequence(int index)       { return _patterns[_selectedPatternIndex].sequence(index); }

    void clear();

    void demoProject();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    fs::Error write(const char *path) const;
    fs::Error read(const char *path);

private:
    uint8_t _slot = uint8_t(-1);
    char _name[NameLength + 1] = "INIT";
    float _bpm = 120.f;
    uint8_t _swing = 50;
    uint8_t _globalMeasure = 1;

    ClockSetup _clockSetup;
    TrackSetupArray _trackSetups;
    PlayState _playState;
    PatternArray _patterns;

    int _selectedTrackIndex = 0;
    int _selectedPatternIndex = 0;
};
