#pragma once

#include "Config.h"
#include "Observable.h"
#include "Types.h"
#include "TimeSignature.h"
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

    // autoLoaded

    bool autoLoaded() const { return _autoLoaded != 0; }
    void setAutoLoaded(bool autoLoaded) { _autoLoaded = autoLoaded ? 1 : 0; }

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
        printRouted(str, Routing::Target::Tempo);
        str("%.1f", tempo());
    }

    // swing

    int swing() const { return _swing.get(isRouted(Routing::Target::Swing)); }
    void setSwing(int swing, bool routed = false) {
        _swing.set(clamp(swing, 50, 75), routed);
    }

    void editSwing(int value, bool shift) {
        if (!isRouted(Routing::Target::Swing)) {
            setSwing(ModelUtils::adjustedByStep(swing(), value, 5, !shift));
        }
    }

    void printSwing(StringBuilder &str) const {
        printRouted(str, Routing::Target::Swing);
        str("%d%%", swing());
    }

    // timeSignature

    TimeSignature timeSignature() const { return _timeSignature; }
    void setTimeSignature(TimeSignature timeSignature) {
        _timeSignature = timeSignature;
    }

    void editTimeSignature(int value, bool shift) {
        _timeSignature.edit(value, shift);
    }

    void printTimeSignature(StringBuilder &str) const {
        _timeSignature.print(str);
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
        str("%d %s", syncMeasure(), syncMeasure() > 1 ? "bars" : "bar");
    }

    // always sync

    bool alwaysSyncPatterns() const { return _alwaysSyncPatterns; }
    void setAlwaysSyncPatterns(bool alwaysSync) {
        _alwaysSyncPatterns = alwaysSync;
    }

    void editAlwaysSyncPatterns(int value, bool shift) {
        _alwaysSyncPatterns = value == 1;
    }

    void printAlwaysSyncPatterns(StringBuilder &str) const {
        if (_alwaysSyncPatterns) str("Always");
        else str("Default");
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

    // monitorMode

    Types::MonitorMode monitorMode() const { return _monitorMode; }
    void setMonitorMode(Types::MonitorMode monitorMode) {
        _monitorMode = ModelUtils::clampedEnum(monitorMode);
    }

    void editMonitorMode(int value, bool shift) {
        _monitorMode = ModelUtils::adjustedEnum(_monitorMode, value);
    }

    void printMonitorMode(StringBuilder &str) const {
        str(Types::monitorModeName(_monitorMode));
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

    // midiInput

    void editMidiInput(int value, bool shift) {
        if (_midiInputMode == Types::MidiInputMode::Source) {
            if (value < 0 && _midiInputSource.isFirst()) {
                _midiInputMode = ModelUtils::adjustedEnum(_midiInputMode, value);
            } else {
                _midiInputSource.edit(value, shift);
            }
        } else {
            _midiInputMode = ModelUtils::adjustedEnum(_midiInputMode, value);
        }
    }

    void printMidiInput(StringBuilder &str) const {
        switch (_midiInputMode) {
        case Types::MidiInputMode::Off:     str("Off"); break;
        case Types::MidiInputMode::All:     str("All"); break;
        case Types::MidiInputMode::Source:  _midiInputSource.print(str); break;
        case Types::MidiInputMode::Last:    break;
        }
    }

    Types::MidiInputMode midiInputMode() const { return _midiInputMode; }
    void setMidiInputMode(Types::MidiInputMode midiInputMode) {
        _midiInputMode = ModelUtils::clampedEnum(midiInputMode);
    }

    const MidiSourceConfig &midiInputSource() const { return _midiInputSource; }
          MidiSourceConfig &midiInputSource()       { return _midiInputSource; }

    // midiIntegrationMode

    void editMidiIntegrationMode(int value, bool shift) {
        _midiIntegrationMode = ModelUtils::adjustedEnum(_midiIntegrationMode, value);
    }

    void printMidiIntegrationMode(StringBuilder &str) const {
        str(Types::midiIntegrationModeName(_midiIntegrationMode));
    }

    void setMidiIntegrationMode(Types::MidiIntegrationMode midiIntegrationMode) {
        _midiIntegrationMode = midiIntegrationMode;
    }

    bool midiIntegrationProgramChangesEnabled() const {
        return _midiIntegrationMode == Types::MidiIntegrationMode::ProgramChanges
               || _midiIntegrationMode == Types::MidiIntegrationMode::Malekko;
    }

    bool midiIntegrationMalekkoEnabled() const {
        return _midiIntegrationMode == Types::MidiIntegrationMode::Malekko;
    }

    // midiProgramOffset

    void editMidiProgramOffset(int value, bool shift) {
        _midiProgramOffset += value;
    }

    void printMidiProgramOffset(StringBuilder &str) const {
        str("%d", _midiProgramOffset);
    }

    void setMidiProgramOffset(int midiProgramOffset) {
        _midiProgramOffset = midiProgramOffset;
    }

    int midiProgramOffset() {
        return _midiProgramOffset;
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

    // curveCvInput

    Types::CurveCvInput curveCvInput() const { return _curveCvInput; }
    void setCurveCvInput(Types::CurveCvInput curveCvInput) {
        _curveCvInput = ModelUtils::clampedEnum(curveCvInput);
    }

    void editCurveCvInput(int value, bool shift) {
        _curveCvInput = ModelUtils::adjustedEnum(_curveCvInput, value);
    }

    void printCurveCvInput(StringBuilder &str) const {
        str(Types::curveCvInput(_curveCvInput));
    }

    // curveMidiInput

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

    inline bool isRouted(Routing::Target target) const { return Routing::isRouted(target); }
    inline void printRouted(StringBuilder &str, Routing::Target target) const { Routing::printRouted(str, target); }
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

    void write(VersionedSerializedWriter &writer) const;
    bool read(VersionedSerializedReader &reader);

private:
    uint8_t _slot = uint8_t(-1);
    char _name[NameLength + 1];
    mutable uint8_t _autoLoaded = 0;
    Routable<float> _tempo;
    Routable<uint8_t> _swing;
    TimeSignature _timeSignature;
    uint8_t _syncMeasure;
    bool _alwaysSyncPatterns;
    uint8_t _scale;
    uint8_t _rootNote;
    Types::RecordMode _recordMode;
    Types::MonitorMode _monitorMode;
    Types::MidiInputMode _midiInputMode;
    MidiSourceConfig _midiInputSource;
    Types::MidiIntegrationMode _midiIntegrationMode;
    uint8_t _midiProgramOffset;
    Types::CvGateInput _cvGateInput;
    Types::CurveCvInput _curveCvInput;

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
