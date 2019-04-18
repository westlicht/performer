#pragma once

#include "Config.h"
#include "Observable.h"
#include "Types.h"
#include "ClockSetup.h"
#include "Track.h"
#include "Song.h"
#include "PlayState.h"
#include "UserScale.h"
#include "Routing.h"
#include "MidiOutput.h"
#include "Serialize.h"
#include "FileDefs.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"
#include "core/utils/StringUtils.h"

class Project {
public:
    // added NoteTrack::cvUpdateMode
    static constexpr uint32_t Version4 = 4;

    // added storing user scales with project
    // added Project::name
    // added UserScale::name
    static constexpr uint32_t Version5 = 5;

    // added Project::cvGateInput
    static constexpr uint32_t Version6 = 6;

    static constexpr uint32_t Version = Version6;

    //----------------------------------------
    // Types
    //----------------------------------------

    static constexpr size_t NameLength = FileHeader::NameLength;

    typedef std::array<Track, CONFIG_TRACK_COUNT> TrackArray;
    typedef std::array<uint8_t, CONFIG_CHANNEL_COUNT> CvOutputTrackArray;
    typedef std::array<uint8_t, CONFIG_CHANNEL_COUNT> GateOutputArray;

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

    // tempo

    float tempo() const { return _tempo.get(isRouted(Routing::Target::Tempo)); }
    void setTempo(float tempo, bool routed = false) {
        _tempo.set(clamp(tempo, 1.f, 1000.f), routed);
    }

    void editTempo(int value, bool shift) {
        if (!isRouted(Routing::Target::Tempo)) {
            setTempo(tempo() + value * (shift ? 0.1f : 1.f));
        }
    }

    void printTempo(StringBuilder &str) const {
        _routed.print(str, Routing::Target::Tempo);
        str("%.1f", tempo());
    }

    // swing

    int swing() const { return _swing.get(isRouted(Routing::Target::Swing)); }
    void setSwing(int swing, bool routed = false) {
        _swing.set(clamp(swing, 50, 75), routed);
    }

    void editSwing(int value, bool shift) {
        if (!isRouted(Routing::Target::Tempo)) {
            setSwing(ModelUtils::adjustedByStep(swing(), value, 5, !shift));
        }
    }

    void printSwing(StringBuilder &str) const {
        _routed.print(str, Routing::Target::Swing);
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

    // scale

    int scale() const { return _scale; }
    void setScale(int scale) {
        _scale = clamp(scale, 0, Scale::Count - 1);
    }

    void editScale(int value, bool shift) {
        setScale(scale() + value);
    }

    void printScale(StringBuilder &str) const {
        str(Scale::name(scale()));
    }

    const Scale &selectedScale() const {
        return Scale::get(scale());
    }

    // rootNote

    int rootNote() const { return _rootNote; }
    void setRootNote(int rootNote) {
        _rootNote = clamp(rootNote, 0, 11);
    }

    void editRootNote(int value, bool shift) {
        setRootNote(rootNote() + value);
    }

    void printRootNote(StringBuilder &str) const {
        Types::printNote(str, _rootNote);
    }

    // recordMode

    Types::RecordMode recordMode() const { return _recordMode; }
    void setRecordMode(Types::RecordMode recordMode) {
        _recordMode = ModelUtils::clampedEnum(recordMode);
    }

    void editRecordMode(int value, bool shift) {
        _recordMode = ModelUtils::adjustedEnum(_recordMode, value);
    }

    void printRecordMode(StringBuilder &str) const {
        str(Types::recordModeName(_recordMode));
    }

    // cvGateInput

    Types::CvGateInput cvGateInput() const { return _cvGateInput; }
    void setCvGateInput(Types::CvGateInput cvGateInput) {
        _cvGateInput = ModelUtils::clampedEnum(cvGateInput);
    }

    void editCvGateInput(int value, bool shift) {
        _cvGateInput = ModelUtils::adjustedEnum(_cvGateInput, value);
    }

    void printCvGateInput(StringBuilder &str) const {
        str(Types::cvGateInputName(_cvGateInput));
    }

    // clockSetup

    const ClockSetup &clockSetup() const { return _clockSetup; }
          ClockSetup &clockSetup()       { return _clockSetup; }

    // tracks

    const TrackArray &tracks() const { return _tracks; }
          TrackArray &tracks()       { return _tracks; }

    const Track &track(int index) const { return _tracks[index]; }
          Track &track(int index)       { return _tracks[index]; }

    // cvOutputTrack

    const CvOutputTrackArray &cvOutputTracks() const { return _cvOutputTracks; }
          CvOutputTrackArray &cvOutputTracks()       { return _cvOutputTracks; }

    int cvOutputTrack(int index) const { return _cvOutputTracks[index]; }
    void setCvOutputTrack(int index, int trackIndex) { _cvOutputTracks[index] = clamp(trackIndex, 0, CONFIG_TRACK_COUNT - 1); }

    void editCvOutputTrack(int index, int value, bool shift) {
        setCvOutputTrack(index, cvOutputTrack(index) + value);
    }

    // gateOutputTrack

    const GateOutputArray &gateOutputTracks() const { return _gateOutputTracks; }
          GateOutputArray &gateOutputTracks()       { return _gateOutputTracks; }

    int gateOutputTrack(int index) const { return _gateOutputTracks[index]; }
    void setGateOutputTrack(int index, int trackIndex) { _gateOutputTracks[index] = clamp(trackIndex, 0, CONFIG_TRACK_COUNT - 1); }

    void editGateOutputTrack(int index, int value, bool shift) {
        setGateOutputTrack(index, gateOutputTrack(index) + value);
    }

    // song

    const Song &song() const { return _song; }
          Song &song()       { return _song; }

    // playState

    const PlayState &playState() const { return _playState; }
          PlayState &playState()       { return _playState; }

    // userScales

    const UserScale::Array &userScales() const { return UserScale::userScales; }
          UserScale::Array &userScales()       { return UserScale::userScales; }

    const UserScale &userScale(int index) const { return UserScale::userScales[index]; }
          UserScale &userScale(int index)       { return UserScale::userScales[index]; }

    // routing

    const Routing &routing() const { return _routing; }
          Routing &routing()       { return _routing; }

    // midiOutput

    const MidiOutput &midiOutput() const { return _midiOutput; }
          MidiOutput &midiOutput()       { return _midiOutput; }

    // selectedTrackIndex

    int selectedTrackIndex() const { return _selectedTrackIndex; }
    void setSelectedTrackIndex(int index) {
        index = clamp(index, 0, CONFIG_TRACK_COUNT - 1);
        if (index != _selectedTrackIndex) {
            _selectedTrackIndex = index;
            _observable.notify(SelectedTrackIndexChanged);

            // switch selected pattern
            setSelectedPatternIndex(_playState.trackState(index).pattern());
        }
    }

    bool isSelectedTrack(int index) const { return _selectedTrackIndex == index; }

    // selectedPatternIndex

    int selectedPatternIndex() const {
        return _playState.snapshotActive() ? PlayState::SnapshotPatternIndex : _selectedPatternIndex;
    }

    void setSelectedPatternIndex(int index) {
        _selectedPatternIndex = clamp(index, 0, CONFIG_PATTERN_COUNT - 1);
        _observable.notify(SelectedPatternIndexChanged);
    }

    bool isSelectedPattern(int index) const { return _selectedPatternIndex == index; }

    void editSelectedPatternIndex(int value, bool shift) {
        setSelectedPatternIndex(selectedPatternIndex() + value);
    }

    // selectedNoteSequenceLayer

    NoteSequence::Layer selectedNoteSequenceLayer() const { return _selectedNoteSequenceLayer; }
    void setSelectedNoteSequenceLayer(NoteSequence::Layer layer) { _selectedNoteSequenceLayer = layer; }

    // selectedCurveSequenceLayer

    CurveSequence::Layer selectedCurveSequenceLayer() const { return _selectedCurveSequenceLayer; }
    void setSelectedCurveSequenceLayer(CurveSequence::Layer layer) { _selectedCurveSequenceLayer = layer; }

    // selectedTrack

    const Track &selectedTrack() const { return _tracks[_selectedTrackIndex]; }
          Track &selectedTrack()       { return _tracks[_selectedTrackIndex]; }

    // noteSequence

    const NoteSequence &noteSequence(int trackIndex, int patternIndex) const { return _tracks[trackIndex].noteTrack().sequence(patternIndex); }
          NoteSequence &noteSequence(int trackIndex, int patternIndex)       { return _tracks[trackIndex].noteTrack().sequence(patternIndex); }

    // selectedNoteSequence

    const NoteSequence &selectedNoteSequence() const { return noteSequence(_selectedTrackIndex, selectedPatternIndex()); }
          NoteSequence &selectedNoteSequence()       { return noteSequence(_selectedTrackIndex, selectedPatternIndex()); }

    // curveSequence

    const CurveSequence &curveSequence(int trackIndex, int patternIndex) const { return _tracks[trackIndex].curveTrack().sequence(patternIndex); }
          CurveSequence &curveSequence(int trackIndex, int patternIndex)       { return _tracks[trackIndex].curveTrack().sequence(patternIndex); }

    // selectedCurveSequence

    const CurveSequence &selectedCurveSequence() const { return curveSequence(_selectedTrackIndex, selectedPatternIndex()); }
          CurveSequence &selectedCurveSequence()       { return curveSequence(_selectedTrackIndex, selectedPatternIndex()); }

    //----------------------------------------
    // Routing
    //----------------------------------------

    inline bool isRouted(Routing::Target target) const { return _routed.has(target); }
    inline void setRouted(Routing::Target target, bool routed) { _routed.set(target, routed); }
    void writeRouted(Routing::Target target, int intValue, float floatValue);

    //----------------------------------------
    // Observable
    //----------------------------------------

    enum Event {
        ProjectCleared,
        ProjectRead,
        TrackModeChanged,
        SelectedTrackIndexChanged,
        SelectedPatternIndexChanged,
    };

    void watch(std::function<void(Event)> handler) {
        _observable.watch(handler);
    }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear();
    void clearPattern(int patternIndex);

    void setTrackMode(int trackIndex, Track::TrackMode trackMode);

    void write(WriteContext &context) const;
    bool read(ReadContext &context);

    fs::Error write(const char *path) const;
    fs::Error read(const char *path);

private:
    uint8_t _slot = uint8_t(-1);
    char _name[NameLength + 1];
    Routable<float> _tempo;
    Routable<uint8_t> _swing;
    uint8_t _syncMeasure;
    uint8_t _scale;
    uint8_t _rootNote;
    Types::RecordMode _recordMode;
    Types::CvGateInput _cvGateInput;

    RoutableSet<Routing::Target::ProjectFirst, Routing::Target::ProjectLast> _routed;

    ClockSetup _clockSetup;
    TrackArray _tracks;
    CvOutputTrackArray _cvOutputTracks;
    GateOutputArray _gateOutputTracks;
    Song _song;
    PlayState _playState;
    Routing _routing;
    MidiOutput _midiOutput;

    int _selectedTrackIndex = 0;
    int _selectedPatternIndex = 0;
    NoteSequence::Layer _selectedNoteSequenceLayer = NoteSequence::Layer(0);
    CurveSequence::Layer _selectedCurveSequenceLayer = CurveSequence::Layer(0);

    Observable<Event, 2> _observable;
};
