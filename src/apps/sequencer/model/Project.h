#pragma once

#include "Config.h"
#include "Types.h"
#include "ClockSetup.h"
#include "TrackSetup.h"
#include "Sequence.h"
#include "PlayState.h"
#include "Serialize.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"
#include "core/utils/StringUtils.h"

class Project {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    static constexpr size_t NameLength = 16;

    typedef std::array<TrackSetup, CONFIG_TRACK_COUNT> TrackSetupArray;
    typedef std::array<Sequence, CONFIG_PATTERN_COUNT> SequenceArray;
    typedef std::array<SequenceArray, CONFIG_TRACK_COUNT> TrackSequenceArray;

    Project();

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

    void editBpm(int value, bool shift) {
        setBpm(bpm() + value * (shift ? 0.1f : 1.f));
    }

    void printBpm(StringBuilder &str) const {
        str("%.1f", bpm());
    }

    // swing

    int swing() const { return _swing; }
    void setSwing(int swing) {
        _swing = clamp(swing, 50, 75);
    }

    void editSwing(int value, bool shift) {
        setSwing(ModelUtils::adjustedByStep(swing(), value, 5, shift));
    }

    void printSwing(StringBuilder &str) const {
        str("%d%%", swing());
    }

    // syncMeasure

    int syncMeasure() const { return _syncMeasure; }
    void setSyncMeasure(int syncMeasure) {
        _syncMeasure = clamp(syncMeasure, 1, 128);
    }

    void editSyncMeasure(int value, bool shift) {
        setSyncMeasure(ModelUtils::adjustedByPowerOfTwo(syncMeasure(), value, shift));
    }

    void printSyncMeasure(StringBuilder &str) const {
        str("%d", syncMeasure());
    }

    // clockSetup

    const ClockSetup &clockSetup() const { return _clockSetup; }
          ClockSetup &clockSetup()       { return _clockSetup; }

    // trackSetups

    const TrackSetupArray &trackSetups() const { return _trackSetups; }
          TrackSetupArray &trackSetups()       { return _trackSetups; }

    const TrackSetup &trackSetup(int index) const { return _trackSetups[index]; }
          TrackSetup &trackSetup(int index)       { return _trackSetups[index]; }

    // playState

    const PlayState &playState() const { return _playState; }
          PlayState &playState()       { return _playState; }

    // selectedTrackIndex

    int selectedTrackIndex() const { return _selectedTrackIndex; }
    void setSelectedTrackIndex(int index) {
        _selectedTrackIndex = clamp(index, 0, CONFIG_TRACK_COUNT - 1);
    }

    bool isSelectedTrack(int index) const { return _selectedTrackIndex == index; }

    // selectedPatternIndex

    int selectedPatternIndex() const { return _selectedPatternIndex; }
    void setSelectedPatternIndex(int index) {
        _selectedPatternIndex = clamp(index, 0, CONFIG_PATTERN_COUNT - 1);
    }

    bool isSelectedPattern(int index) const { return _selectedPatternIndex == index; }

    void editSelectedPatternIndex(int value, bool shift) {
        setSelectedPatternIndex(selectedPatternIndex() + value);
    }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void setTrackSetup(int trackIndex, const TrackSetup &trackSetup);
    void setTrackMode(int trackIndex, Types::TrackMode trackMode);

    const TrackSetup &selectedTrackSetup() const { return _trackSetups[_selectedTrackIndex]; }
          TrackSetup &selectedTrackSetup()       { return _trackSetups[_selectedTrackIndex]; }

    const Sequence &sequence(int trackIndex, int patternIndex) const { return _sequences[trackIndex][patternIndex]; }
          Sequence &sequence(int trackIndex, int patternIndex)       { return _sequences[trackIndex][patternIndex]; }

    const Sequence &selectedSequence() const { return sequence(_selectedTrackIndex, _selectedPatternIndex); }
          Sequence &selectedSequence()       { return sequence(_selectedTrackIndex, _selectedPatternIndex); }

    void clear();
    void clearPattern(int patternIndex);

    void demoProject();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    fs::Error write(const char *path) const;
    fs::Error read(const char *path);

private:
    uint8_t _slot = uint8_t(-1);
    char _name[NameLength + 1] = { 'I', 'N', 'I', 'T', '\0' };
    float _bpm = 120.f;
    uint8_t _swing = 50;
    uint8_t _syncMeasure = 1;

    ClockSetup _clockSetup;
    TrackSetupArray _trackSetups;
    TrackSequenceArray _sequences;
    PlayState _playState;

    int _selectedTrackIndex = 0;
    int _selectedPatternIndex = 0;
};
