#include "Engine.h"

#include "Config.h"
#include "MidiUtils.h"

#include "core/Debug.h"
#include "core/midi/MidiMessage.h"

#include "os/os.h"

Engine::Engine(Model &model, ClockTimer &clockTimer, Adc &adc, Dac &dac, Dio &dio, GateOutput &gateOutput, Midi &midi, UsbMidi &usbMidi) :
    _model(model),
    _project(model.project()),
    _dio(dio),
    _gateOutput(gateOutput),
    _midi(midi),
    _usbMidi(usbMidi),
    _cvInput(adc),
    _cvOutput(dac, model.settings().calibration()),
    _clock(clockTimer),
    _midiOutputEngine(*this, model),
    _routingEngine(*this, model)
{
    _cvOutputOverrideValues.fill(0.f);
    _trackEngines.fill(nullptr);

    _usbMidi.setConnectHandler([this] (uint16_t vendorId, uint16_t productId) { usbMidiConnect(vendorId, productId); });
    _usbMidi.setDisconnectHandler([this] () { usbMidiDisconnect(); });

    _midiMonitoring.inputChanged(_project);
}

void Engine::init() {
    _cvInput.init();
    _cvOutput.init();
    _clock.init();

    initClock();
    updateClockSetup();

    // setup track engines
    updateTrackSetups();
    reset();

    _lastSystemTicks = os::ticks();
}

void Engine::update() {
    // locking
    _locked = _requestLock;
    if (_locked) {
        return;
    }

    uint32_t systemTicks = os::ticks();
    float dt = (0.001f * (systemTicks - _lastSystemTicks)) / os::time::ms(1);
    _lastSystemTicks = systemTicks;

    // suspending
    if (_requestSuspend != _suspended) {
        if (_requestSuspend) {
            _clock.masterStop();
        }
        _suspended = _requestSuspend;
    }

    if (_suspended) {
        // consume ticks
        uint32_t tick;
        while (_clock.checkTick(&tick)) {}

        // consume midi events
        uint8_t cable;
        MidiMessage message;
        while (_midi.recv(&message)) {}
        while (_usbMidi.recv(&cable, &message)) {}

        _cvInput.update();
        updateOverrides();
        _cvOutput.update();
        _gateOutput.update();
        return;
    }

    // process clock events
    while (Clock::Event event = _clock.checkEvent()) {
        switch (event) {
        case Clock::Start:
            // DBG("START");
            reset();
            _state.setRunning(true);
            break;
        case Clock::Stop:
            // DBG("STOP");
            _state.setRunning(false);
            break;
        case Clock::Continue:
            // DBG("CONTINUE");
            _state.setRunning(true);
            break;
        case Clock::Reset:
            // DBG("RESET");
            reset();
            _state.setRunning(false);
            break;
        }
    }

    // update tempo
    _nudgeTempo.update(dt);
    _clock.setMasterBpm(_project.tempo() * (1.f + _nudgeTempo.strength() * 0.1f));

    // update clock setup
    updateClockSetup();

    // update track setups
    updateTrackSetups();

    // update play state
    updatePlayState(false);

    // update cv inputs
    _cvInput.update();

    // receive midi events
    receiveMidi();

    // update routings
    _routingEngine.update();

    uint32_t tick;
    while (_clock.checkTick(&tick)) {
        _tick = tick;

        // update play state
        updatePlayState(true);

        // tick track engines
        for (size_t trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            auto &trackEngine = _trackEngines[trackIndex];
            uint32_t result = trackEngine->tick(tick);
            // update track outputs and routings if tick results in updating the track's CV output
            if (result &= TrackEngine::TickResult::CvUpdate && _trackUpdateReducers[trackIndex].update()) {
                trackEngine->update(0.f);
                updateTrackOutputs();
                updateOverrides();
                _routingEngine.update();
            }
        }

        // update midi outputs, force sending CC on first tick
        if (tick == 0) {
            _midiOutputEngine.update(true);
        }
    }

    for (auto trackEngine : _trackEngines) {
        trackEngine->update(dt);
    }

    _midiOutputEngine.update();

    updateTrackOutputs();
    updateOverrides();

    // update cv/gate outputs
    _cvOutput.update();
    _gateOutput.update();
}

void Engine::lock() {
    while (!isLocked()) {
        _requestLock = 1;
#ifdef PLATFORM_SIM
        update();
#endif
    }
}

void Engine::unlock() {
    while (isLocked()) {
        _requestLock = 0;
#ifdef PLATFORM_SIM
        update();
#endif
    }
}

void Engine::suspend() {
    // TODO make re-entrant
    while (!isSuspended()) {
        _requestSuspend = 1;
#ifdef PLATFORM_SIM
        update();
#endif
    }
}

void Engine::resume() {
    while (isSuspended()) {
        _requestSuspend = 0;
#ifdef PLATFORM_SIM
        update();
#endif
    }
}

void Engine::togglePlay(bool shift) {
    if (shift) {
        switch (_project.clockSetup().shiftMode()) {
        case ClockSetup::ShiftMode::Restart:
            // restart
            clockStart();
            break;
        case ClockSetup::ShiftMode::Pause:
            // stop/continue
            if (clockRunning()) {
                clockStop();
            } else {
                clockContinue();
            }
            break;
        case ClockSetup::ShiftMode::Last:
            break;
        }
    } else {
        // start/stop
        if (clockRunning()) {
            clockReset();
        } else {
            clockStart();
        }
    }
}

void Engine::clockStart() {
    _clock.masterStart();
}

void Engine::clockStop() {
    _clock.masterStop();
}

void Engine::clockContinue() {
    _clock.masterContinue();
}

void Engine::clockReset() {
    _clock.masterReset();
}

bool Engine::clockRunning() const {
    return _state.running();
}

void Engine::toggleRecording() {
    _state.setRecording(!_state.recording());
}

void Engine::setRecording(bool recording) {
    _state.setRecording(recording);
}

bool Engine::recording() const {
    return _state.recording();
}

void Engine::tapTempoReset() {
    _tapTempo.reset();
}

void Engine::tapTempoTap() {
    float bpm = _project.tempo();
    bpm = _tapTempo.tap(bpm);
    _project.setTempo(bpm);
}

void Engine::nudgeTempoSetDirection(int direction) {
    _nudgeTempo.setDirection(direction);
}

float Engine::nudgeTempoStrength() const {
    return _nudgeTempo.strength();
}

uint32_t Engine::noteDivisor() const {
    return _project.timeSignature().noteDivisor();
}

uint32_t Engine::measureDivisor() const {
    return _project.timeSignature().measureDivisor();
}

float Engine::measureFraction() const {
    uint32_t divisor = measureDivisor();
    return float(_tick % divisor) / divisor;
}

uint32_t Engine::syncDivisor() const {
    return _project.syncMeasure() * measureDivisor();
}

float Engine::syncFraction() const {
    uint32_t divisor = syncDivisor();
    return float(_tick % divisor) / divisor;
}

bool Engine::trackEnginesConsistent() const {
    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        if (trackEngine(trackIndex).trackMode() != _project.track(trackIndex).trackMode()) {
            return false;
        }
    }
    return true;
}

bool Engine::trackPatternsConsistent() const {
    auto playState = _project.playState();
    auto firstTrackState = playState.trackState(0);

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        auto trackState = playState.trackState(trackIndex);

        if (trackState.pattern() != firstTrackState.pattern()
            || trackState.requestedPattern() != firstTrackState.requestedPattern()) {
            return false;
        }
    }
    return true;
}

bool Engine::sendMidi(MidiPort port, uint8_t cable, const MidiMessage &message) {
    switch (port) {
    case MidiPort::Midi:
        return _midi.send(message);
    case MidiPort::UsbMidi:
        return _usbMidi.send(cable, message);
    case MidiPort::CvGate:
        // input only
        break;
    }
    return false;
}

bool Engine::midiProgramChangesEnabled() {
    return _project.midiIntegrationProgramChangesEnabled()
        && trackPatternsConsistent()
        && !_project.playState().snapshotActive();
}

void Engine::sendMidiProgramChange(int programNumber) {
    if (_project.midiIntegrationMalekkoEnabled()) {
        _midiOutputEngine.sendMalekkoSelectHandshake(0);
    }
    if (_project.midiIntegrationProgramChangesEnabled()) {
        _midiOutputEngine.sendProgramChange(0, _project.midiProgramOffset() + programNumber);
    }
    if (_project.midiIntegrationMalekkoEnabled()) {
        _midiOutputEngine.sendMalekkoSelectReleaseHandshake(0);
    }
}

void Engine::sendMidiProgramSave(int programNumber) {
    if (_project.midiIntegrationMalekkoEnabled()) {
        _midiOutputEngine.sendMalekkoSaveHandshake(0);
    }
    if (_project.midiIntegrationProgramChangesEnabled()) {
        _midiOutputEngine.sendProgramChange(0, _project.midiProgramOffset() + programNumber);
    }
}

void Engine::showMessage(const char *text, uint32_t duration) {
    if (_messageHandler) {
        _messageHandler(text, duration);
    }
}

void Engine::setMessageHandler(MessageHandler handler) {
    _messageHandler = handler;
}

Engine::Stats Engine::stats() const {
    return {
        .uptime = os::ticks() / os::time::ms(1000),
        .midiRxOverflow = _midi.rxOverflow(),
        .usbMidiRxOverflow = _usbMidi.rxOverflow()
    };
}

void Engine::onClockOutput(const Clock::OutputState &state) {
    _dio.clockOutput.set(state.clock);
    switch (_project.clockSetup().clockOutputMode()) {
    case ClockSetup::ClockOutputMode::Reset:
        _dio.resetOutput.set(state.reset);
        break;
    case ClockSetup::ClockOutputMode::Run:
        _dio.resetOutput.set(state.run);
        break;
    case ClockSetup::ClockOutputMode::Last:
        break;
    }
}

void Engine::onClockMidi(uint8_t data) {
    // TODO we should send a single byte with priority
    const auto &clockSetup = _project.clockSetup();
    if (clockSetup.midiTx()) {
        _midi.send(MidiMessage(data));
    }
    if (clockSetup.usbTx()) {
        // always send clock on cable 0
        _usbMidi.send(0, MidiMessage(data));
    }
}

void Engine::updateTrackSetups() {
    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        auto &track = _project.track(trackIndex);
        int linkTrack = track.linkTrack();
        const TrackEngine *linkedTrackEngine = linkTrack >= 0 ? &trackEngine(linkTrack) : nullptr;

        if (!_trackEngines[trackIndex] || _trackEngines[trackIndex]->trackMode() != track.trackMode()) {
            auto &trackEngine = _trackEngines[trackIndex];
            auto &trackContainer = _trackEngineContainers[trackIndex];

            switch (track.trackMode()) {
            case Track::TrackMode::Note:
                trackEngine = trackContainer.create<NoteTrackEngine>(*this, _model, track, linkedTrackEngine);
                break;
            case Track::TrackMode::Curve:
                trackEngine = trackContainer.create<CurveTrackEngine>(*this, _model, track, linkedTrackEngine);
                break;
            case Track::TrackMode::MidiCv:
                trackEngine = trackContainer.create<MidiCvTrackEngine>(*this, _model, track, linkedTrackEngine);
                break;
            case Track::TrackMode::Last:
                break;
            }
        }

        // update linked track engine
        _trackEngines[trackIndex]->setLinkedTrackEngine(linkedTrackEngine);
    }
}

void Engine::updateTrackOutputs() {
    const auto &gateOutputTracks = _project.gateOutputTracks();
    const auto &cvOutputTracks = _project.cvOutputTracks();

    int trackGateIndex[CONFIG_TRACK_COUNT];
    int trackCvIndex[CONFIG_TRACK_COUNT];

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        trackGateIndex[trackIndex] = 0;
        trackCvIndex[trackIndex] = 0;
    }

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        int gateOutputTrack = gateOutputTracks[trackIndex];
        if (!_gateOutputOverride) {
            _gateOutput.setGate(trackIndex, _trackEngines[gateOutputTrack]->gateOutput(trackGateIndex[gateOutputTrack]++));
        }
        int cvOutputTrack = cvOutputTracks[trackIndex];
        if (!_cvOutputOverride) {
            _cvOutput.setChannel(trackIndex, _trackEngines[cvOutputTrack]->cvOutput(trackCvIndex[cvOutputTrack]++));
        }
    }
}

void Engine::reset() {
    for (auto trackEngine : _trackEngines) {
        trackEngine->reset();
    }

    _midiOutputEngine.reset();
}

void Engine::updatePlayState(bool ticked) {
    auto &playState = _project.playState();
    auto &songState = playState.songState();
    const auto &song = _project.song();

    bool hasImmediateRequests = playState.hasImmediateRequests();
    bool hasSyncedRequests = playState.hasSyncedRequests();
    bool handleLatchedRequests = playState.executeLatchedRequests();
    bool hasRequests = hasImmediateRequests || hasSyncedRequests || handleLatchedRequests;

    bool handleSyncedRequests = _tick % syncDivisor() == 0;
    bool handleSongAdvance = ticked && _tick > 0 && _tick % measureDivisor() == 0;
    bool withinPreHandleRange = (_tick + 192) % syncDivisor() < 192;
    if (withinPreHandleRange && _pendingPreHandle == PreHandleNone) {
        _pendingPreHandle = PreHandlePending;
    } else if (!withinPreHandleRange && _pendingPreHandle != PreHandleNone) {
        _pendingPreHandle = PreHandleNone;
    }

    // send initial program change if we haven't sent it already
    // means that when the sequencer initially starts, it will sync connected devices to the same pattern
    // only works when all patterns are equal
    // we also send a program change if the midi program offset setting is updated
    if (midiProgramChangesEnabled() && (!_midiHasSentInitialPgmChange || _project.midiProgramOffset() != _midiLastInitialProgramOffset)) {
        sendMidiProgramChange(playState.trackState(0).pattern());
        _midiHasSentInitialPgmChange = true;
        _midiLastInitialProgramOffset = _project.midiProgramOffset();
    }

    // handle mute & pattern requests

    bool changedPatterns = false;

    if (hasRequests) {
        int muteRequests = PlayState::TrackState::ImmediateMuteRequest |
            (handleSyncedRequests ? PlayState::TrackState::SyncedMuteRequest : 0) |
            (handleLatchedRequests ? PlayState::TrackState::LatchedMuteRequest : 0);

        int patternRequests = PlayState::TrackState::ImmediatePatternRequest |
            (handleSyncedRequests ? PlayState::TrackState::SyncedPatternRequest : 0) |
            (handleLatchedRequests ? PlayState::TrackState::LatchedPatternRequest : 0);

        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            auto &trackState = playState.trackState(trackIndex);

            // handle mute requests
            if (trackState.hasRequests(muteRequests)) {
                trackState.setMute(trackState.requestedMute());
            }

            // handle pattern requests
            if (trackState.hasRequests(patternRequests)) {
                trackState.setPattern(trackState.requestedPattern());
                changedPatterns = true;
            }

            // clear requests
            trackState.clearRequests(muteRequests | patternRequests);
        }

        bool shouldSendPgmChange = !_preSendMidiPgmChange && changedPatterns;
        bool shouldPreSendPgmChange = _preSendMidiPgmChange && ((changedPatterns && !playState.hasSyncedRequests())
                                                                || (_pendingPreHandle == PreHandlePending && playState.hasSyncedRequests()));

        if (midiProgramChangesEnabled() && (shouldSendPgmChange || shouldPreSendPgmChange)) {
            sendMidiProgramChange(playState.trackState(0).requestedPattern());
            _pendingPreHandle = PreHandleComplete;
        }
    }

    // handle song requests

    auto activateSongSlot = [&] (const Song::Slot &slot) {
        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            playState.trackState(trackIndex).setPattern(slot.pattern(trackIndex));
            // only set mutes if track in song contains any mutes at all
            if (song.trackHasMutes(trackIndex)) {
                playState.trackState(trackIndex).setMute(slot.mute(trackIndex));
            }
        }
    };

    if (hasRequests) {
        int playRequests = PlayState::SongState::ImmediatePlayRequest |
            (handleSyncedRequests ? PlayState::SongState::SyncedPlayRequest : 0) |
            (handleLatchedRequests ? PlayState::SongState::LatchedPlayRequest : 0);

        int stopRequests = PlayState::SongState::ImmediateStopRequest |
            (handleSyncedRequests ? PlayState::SongState::SyncedStopRequest : 0) |
            (handleLatchedRequests ? PlayState::SongState::LatchedStopRequest : 0);

        if (songState.hasRequests(playRequests)) {
            int requestedSlot = songState.requestedSlot();
            if (requestedSlot >= 0 && requestedSlot < song.slotCount()) {
                activateSongSlot(song.slot(requestedSlot));
                songState.setCurrentSlot(requestedSlot);
                songState.setCurrentRepeat(0);
                songState.setPlaying(true);
                handleSongAdvance = false;

                // start clock if not running
                if (!clockRunning()) {
                    clockStart();
                }
            }
        }

        if (changedPatterns || songState.hasRequests(stopRequests)) {
            songState.setPlaying(false);
        }

        songState.clearRequests(playRequests | stopRequests);
    }

    // clear pending requests

    if (hasRequests) {
        playState.clearImmediateRequests();
        if (handleSyncedRequests) {
            playState.clearSyncedRequests();
        }
        if (handleLatchedRequests) {
            playState.clearLatchedRequests();
        }
    }

    // handle song slot change
    if (songState.playing()) {
        const auto &slot = song.slot(songState.currentSlot());
        int currentSlot = songState.currentSlot();
        int currentRepeat = songState.currentRepeat();

        // send program changes when advancing pattern in song mode
        if (ticked && ((_preSendMidiPgmChange && _pendingPreHandle == PreHandlePending) || (!_preSendMidiPgmChange && handleSongAdvance))) {
            if (currentRepeat + 1 >= slot.repeats()) {
                auto nextSlot = song.slot(currentSlot + 1 < song.slotCount() ? currentSlot + 1 : 0);
                bool nextSlotPatternsEqual = true;
                int firstPattern = nextSlot.pattern(0);

                for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
                    if (nextSlot.pattern(trackIndex) != firstPattern) {
                        nextSlotPatternsEqual = false;
                        break;
                    }
                }

                if (midiProgramChangesEnabled() && nextSlotPatternsEqual) {
                    sendMidiProgramChange(firstPattern);
                }
            }

            // TODO There is a possible race condition with synced patterns here
            // If song mode is active, we pre-send the program change,
            // and a user syncs a pattern change after it was pre-sent
            // we will not send the program change for the synced pattern change
            _pendingPreHandle = PreHandleComplete;
        }

        if (handleSongAdvance) {
            if (currentRepeat + 1 < slot.repeats()) {
                // next repeat
                songState.setCurrentRepeat(currentRepeat + 1);
            } else {
                // next slot
                songState.setCurrentRepeat(0);
                if (currentSlot + 1 < song.slotCount()) {
                    songState.setCurrentSlot(currentSlot + 1);
                } else {
                    songState.setCurrentSlot(0);
                }

                // update patterns
                activateSongSlot(song.slot(songState.currentSlot()));
                for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
                    _trackEngines[trackIndex]->restart();
                }
            }
        }
    }

    // abort song mode if slot becomes invalid

    if ((songState.playing() && songState.currentSlot() >= song.slotCount()) ||
        (songState.currentRepeat() >= song.slot(songState.currentSlot()).repeats())) {
        playState.stopSong();
    }

    if (hasRequests | handleSongAdvance) {
        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            _trackEngines[trackIndex]->changePattern();
        }
    }
}

void Engine::updateOverrides() {
    // overrides
    if (_gateOutputOverride) {
        _gateOutput.setGates(_gateOutputOverrideValue);
    }
    if (_cvOutputOverride) {
        for (size_t i = 0; i < _cvOutputOverrideValues.size(); ++i) {
            _cvOutput.setChannel(i, _cvOutputOverrideValues[i]);
        }
    }
}

void Engine::usbMidiConnect(uint16_t vendorId, uint16_t productId) {
    if (_usbMidiConnectHandler) {
        _usbMidiConnectHandler(vendorId, productId);
    }
}

void Engine::usbMidiDisconnect() {
    if (_usbMidiDisconnectHandler) {
        _usbMidiDisconnectHandler();
    }
}

void Engine::receiveMidi() {
    // reset MIDI monitoring if monitoring config has changed
    if (_midiMonitoring.inputChanged(_project)) {
        for (auto trackEngine : _trackEngines) {
            trackEngine->clearMidiMonitoring();
        }
    }

    // receive MIDI messages from ports
    MidiMessage message;
    while (_midi.recv(&message)) {
        message.fixFakeNoteOff();
        receiveMidi(MidiPort::Midi, 0, message);
    }
    uint8_t cable;
    while (_usbMidi.recv(&cable, &message)) {
        message.fixFakeNoteOff();
        receiveMidi(MidiPort::UsbMidi, cable, message);
    }

    // derive MIDI messages from CV/Gate input
    switch (_project.cvGateInput()) {
    case Types::CvGateInput::Off:
        _cvGateToMidiConverter.reset();
        break;
    case Types::CvGateInput::Cv1Cv2:
        _cvGateToMidiConverter.convert(_cvInput.channel(0), _cvInput.channel(1), 0, [this] (const MidiMessage &message) {
            receiveMidi(MidiPort::CvGate, 0, message);
        });
        break;
    case Types::CvGateInput::Cv3Cv4:
        _cvGateToMidiConverter.convert(_cvInput.channel(2), _cvInput.channel(3), 1, [this] (const MidiMessage &message) {
            receiveMidi(MidiPort::CvGate, 0, message);
        });
        break;
    case Types::CvGateInput::Last:
        break;
    }
}

void Engine::receiveMidi(MidiPort port, uint8_t cable, const MidiMessage &message) {
    // filter out real-time and system messages
    if (message.isRealTimeMessage() || message.isSystemMessage()) {
        return;
    }

    // let receive handler consume messages (controllers in UI task)
    if (_midiReceiveHandler) {
        if (_midiReceiveHandler(port, cable, message)) {
            return;
        }
    }

    // discard all messages not from cable 0
    if (cable != 0) {
        return;
    }

    // handle program changes
    if (message.isProgramChange() && _project.midiIntegrationProgramChangesEnabled()) {
        auto &playState = _project.playState();
        // if requested pattern > 16, we wrap around and start from the beginning
        // this allows other gear that may send fixed program changes based on the pattern
        // to still select some sequence on the performer on patterns > 16
        int requestedPattern = message.programNumber() % CONFIG_PATTERN_COUNT;

        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            playState.selectTrackPattern(trackIndex, requestedPattern, PlayState::ExecuteType::Immediate);
        }

        // Forward program changes to output
        _midiOutputEngine.sendProgramChange(0, requestedPattern);
    }

    // let midi learn inspect messages (except from virtual CV/Gate messages)
    if (port != MidiPort::CvGate) {
        _midiLearn.receiveMidi(port, message);
    }

    // let routing engine consume messages
    if (_routingEngine.receiveMidi(port, message)) {
        return;
    }

    // let track engines consume messages (only MIDI/CV tracks)
    // allow all tracks to receive messages even if one of them consumes it
    bool consumed = false;
    for (auto trackEngine : _trackEngines) {
        consumed |= trackEngine->receiveMidi(port, message);
    }
    if (consumed) {
        return;
    }

    // midi monitoring (and recording)
    if (port != MidiPort::CvGate) {
        if (_project.midiInputMode() == Types::MidiInputMode::Off) {
            return;
        }
        if (_project.midiInputMode() == Types::MidiInputMode::Source && !MidiUtils::matchSource(port, message, _project.midiInputSource())) {
            return;
        }
    }
    monitorMidi(message);
}

void Engine::monitorMidi(const MidiMessage &message) {
    // helper to send monitor message to a track engine
    auto sendMidi = [this] (int trackIndex, const MidiMessage &message) {
        _trackEngines[trackIndex]->monitorMidi(_tick, message);
    };

    auto currentTrack = _project.selectedTrackIndex();

    // detect when selected track has changed and a note is still active -> send note off
    if (int(_midiMonitoring.lastNote) != -1 && int(_midiMonitoring.lastTrack) != -1 && currentTrack != _midiMonitoring.lastTrack) {
        sendMidi(_midiMonitoring.lastTrack, MidiMessage::makeNoteOff(0, _midiMonitoring.lastNote));
    }

    if (message.isNoteOn()) {
        // detect if a different note is still active => send note off
        if (_midiMonitoring.lastNote != -1 && _midiMonitoring.lastNote != message.note()) {
            sendMidi(currentTrack, MidiMessage::makeNoteOff(0, _midiMonitoring.lastNote));
        }
        // send note on
        sendMidi(currentTrack, MidiMessage::makeNoteOn(0, message.note(), message.velocity()));
        _midiMonitoring.lastNote = message.note();
        _midiMonitoring.lastTrack = currentTrack;
    } else if (message.isNoteOff()) {
        // send note off
        sendMidi(currentTrack, MidiMessage::makeNoteOff(0, message.note()));
        _midiMonitoring.lastNote = -1;
        _midiMonitoring.lastTrack = currentTrack;
    } else {
        sendMidi(currentTrack, message);
    }
}

void Engine::initClock() {
    _clock.setListener(this);

    const auto &clockSetup = _project.clockSetup();

    // Forward external clock signals to clock
    _dio.clockInput.setHandler([&] (bool value) {
        // interrupt context

        // start clock on first clock pulse if reset is not hold and clock is not running
        if (clockSetup.clockInputMode() == ClockSetup::ClockInputMode::Reset && !_clock.isRunning() && !_dio.resetInput.get()) {
            _clock.slaveStart(ClockSourceExternal);
        }
        if (value) {
            _clock.slaveTick(ClockSourceExternal);
        }
    });

    // Handle reset or start/stop input
    _dio.resetInput.setHandler([&] (bool value) {
        // interrupt context
        switch (clockSetup.clockInputMode()) {
        case ClockSetup::ClockInputMode::Reset:
            if (value) {
                _clock.slaveReset(ClockSourceExternal);
            } else {
                _clock.slaveStart(ClockSourceExternal);
            }
            break;
        case ClockSetup::ClockInputMode::Run:
            if (value) {
                _clock.slaveContinue(ClockSourceExternal);
            } else {
                _clock.slaveStop(ClockSourceExternal);
            }
            break;
        case ClockSetup::ClockInputMode::StartStop:
            if (value) {
                _clock.slaveStart(ClockSourceExternal);
            } else {
                _clock.slaveStop(ClockSourceExternal);
                _clock.slaveReset(ClockSourceExternal);
            }
            break;
        case ClockSetup::ClockInputMode::Last:
            break;
        }
    });

    // Forward MIDI clock messages to clock
    _midi.setRecvFilter([this] (uint8_t data) {
        if (MidiMessage::isClockMessage(data)) {
            _clock.slaveHandleMidi(ClockSourceMidi, data);
            return true;
        }
        return false;
    });

    _usbMidi.setRecvFilter([this] (uint8_t data) {
        if (MidiMessage::isClockMessage(data)) {
            _clock.slaveHandleMidi(ClockSourceUsbMidi, data);
            return true;
        }
        return false;
    });
}

void Engine::updateClockSetup() {
    auto &clockSetup = _project.clockSetup();

    // Update clock swing
    _clock.outputConfigureSwing(clockSetup.clockOutputSwing() ? _project.swing() : 0);

    if (!clockSetup.isDirty()) {
        return;
    }

    // Configure clock mode
    switch (clockSetup.mode()) {
    case ClockSetup::Mode::Auto:
        _clock.setMode(Clock::Mode::Auto);
        break;
    case ClockSetup::Mode::Master:
        _clock.setMode(Clock::Mode::Master);
        break;
    case ClockSetup::Mode::Slave:
        _clock.setMode(Clock::Mode::Slave);
        break;
    case ClockSetup::Mode::Last:
        break;
    }

    // Configure clock slaves
    _clock.slaveConfigure(ClockSourceExternal, clockSetup.clockInputDivisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN), true);
    _clock.slaveConfigure(ClockSourceMidi, CONFIG_PPQN / 24, clockSetup.midiRx());
    _clock.slaveConfigure(ClockSourceUsbMidi, CONFIG_PPQN / 24, clockSetup.usbRx());

    // Update from clock input signal
    bool resetInput = _dio.resetInput.get();
    bool running = _clock.isRunning();

    switch (clockSetup.clockInputMode()) {
    case ClockSetup::ClockInputMode::Reset:
        if (resetInput && running) {
            _clock.slaveReset(ClockSourceExternal);
        }
        break;
    case ClockSetup::ClockInputMode::Run:
        if (resetInput && !running) {
            _clock.slaveContinue(ClockSourceExternal);
        } else if (!resetInput && running) {
            _clock.slaveStop(ClockSourceExternal);
        }
        break;
    case ClockSetup::ClockInputMode::StartStop:
        if (resetInput && !running) {
            _clock.slaveStart(ClockSourceExternal);
        } else if (!resetInput && running) {
            _clock.slaveReset(ClockSourceExternal);
        }
        break;
    case ClockSetup::ClockInputMode::Last:
        break;
    }

    // Configure clock outputs
    _clock.outputConfigure(clockSetup.clockOutputDivisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN), clockSetup.clockOutputPulse());

    // Update clock outputs
    onClockOutput(_clock.outputState());

    clockSetup.clearDirty();
}
