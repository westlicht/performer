#include "Engine.h"

#include "Config.h"

#include "core/Debug.h"
#include "core/midi/MidiMessage.h"

#include "os/os.h"

Engine::Engine(Model &model, ClockTimer &clockTimer, Adc &adc, Dac &dac, Dio &dio, GateOutput &gateOutput, Midi &midi, UsbMidi &usbMidi) :
    _model(model),
    _dio(dio),
    _gateOutput(gateOutput),
    _midi(midi),
    _usbMidi(usbMidi),
    _cvInput(adc),
    _cvOutput(dac),
    _clock(clockTimer),
    _routingEngine(*this, model),
    _controllerManager(usbMidi)
{
    _cvOutputOverrideValues.fill(0.f);
    _trackEngines.fill(nullptr);
}

void Engine::init() {
    _cvInput.init();
    _cvOutput.init();
    _clock.init();

    initClockSources();
    initClockOutputs();
    updateClockSetup();

    // setup track engines
    updateTrackSetups();
    updateTrackSequences();
    resetTrackEngines();

    _lastSystemTicks = os::ticks();
}

void Engine::update() {
    uint32_t systemTicks = os::ticks();
    float dt = (0.001f * (systemTicks - _lastSystemTicks)) / os::time::ms(1);
    _lastSystemTicks = systemTicks;

    // process clock requests
    if (_clock.checkStart()) {
        DBG("START");
        resetTrackEngines();
        _running = true;
    }

    if (_clock.checkStop()) {
        DBG("STOP");
        _running = false;
    }

    if (_clock.checkResume()) {
        DBG("RESUME");
        _running = true;
    }

    receiveMidi();

    // update tempo
    _nudgeTempo.update(dt);
    _clock.setMasterBpm(_model.project().bpm() + _nudgeTempo.strength() * 10.f);

    // update clock setup
    updateClockSetup();

    // update track setups
    updateTrackSetups();

    // update play state
    updatePlayState();

    // update cv inputs
    _cvInput.update();

    // update routings
    _routingEngine.update();

    uint32_t tick;
    bool updated = false;
    while (_clock.checkTick(&tick)) {
        _tick = tick;

        // update play state
        updatePlayState();

        for (auto trackEngine : _trackEngines) {
            trackEngine->tick(tick);
        }

        updateTrackOutputs();
        updated = true;
    }

    if (!updated) {
        updateTrackOutputs();
    }

    // overrides
    if (_gateOutputOverride) {
        _gateOutput.setGates(_gateOutputOverrideValue);
    }
    if (_cvOutputOverride) {
        for (size_t i = 0; i < _cvOutputOverrideValues.size(); ++i) {
            _cvOutput.setChannel(i, _cvOutputOverrideValues[i]);
        }
    }

    // update cv outputs
    _cvOutput.update();

    // update midi controller
    // _controllerManager.update();
}

void Engine::start() {
    _clock.masterStart();
}

void Engine::stop() {
    _clock.masterStop();
}

void Engine::resume() {
    _clock.masterResume();
}

void Engine::tapTempoReset() {
    _tapTempo.reset(_model.project().bpm());
}

void Engine::tapTempoTap() {
    _tapTempo.tap();
    _model.project().setBpm(_tapTempo.bpm());
}

void Engine::nudgeTempoSetDirection(int direction) {
    _nudgeTempo.setDirection(direction);
}

float Engine::nudgeTempoStrength() const {
    return _nudgeTempo.strength();
}

float Engine::globalMeasureFraction() const {
    uint32_t measureDivisor = (_model.project().syncMeasure() * CONFIG_PPQN * 4);
    return float(_tick % measureDivisor) / measureDivisor;
}

void Engine::showMessage(const char *text, uint32_t duration) {
    if (_messageHandler) {
        _messageHandler(text, duration);
    }
}

void Engine::setMessageHandler(MessageHandler handler) {
    _messageHandler = handler;
}

void Engine::updateClockSetup() {
    auto &clockSetup = _model.project().clockSetup();

    if (!clockSetup.isDirty()) {
        return;
    }

    _clock.slaveConfigure(ClockSourceExternal, clockSetup.clockInputPPQN(), Clock::SlaveFreeRunning);
    _clock.outputConfigure(clockSetup.clockOutputPPQN());

    clockSetup.clearDirty();
}

void Engine::updateTrackSetups() {
    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        const auto &track = _model.project().track(trackIndex);
        int linkTrack = track.linkTrack();
        const TrackEngine *linkedTrackEngine = linkTrack >= 0 ? &trackEngine(linkTrack) : nullptr;

        if (!_trackEngines[trackIndex] || _trackEngines[trackIndex]->trackMode() != track.trackMode()) {
            auto &trackEngine = _trackEngines[trackIndex];
            auto &trackContainer = _trackEngineContainers[trackIndex];

            switch (track.trackMode()) {
            case Track::TrackMode::Note:
                trackEngine = trackContainer.create<NoteTrackEngine>(track, linkedTrackEngine);
                break;
            case Track::TrackMode::Curve:
                trackEngine = trackContainer.create<CurveTrackEngine>(track, linkedTrackEngine);
                break;
            case Track::TrackMode::MidiCv:
                trackEngine = trackContainer.create<MidiCvTrackEngine>(track, linkedTrackEngine);
                break;
            case Track::TrackMode::Last:
                break;
            }

            const auto &trackState = _model.project().playState().trackState(trackIndex);
            trackEngine->setMute(trackState.mute());
            trackEngine->setFill(trackState.fill());
            trackEngine->setPattern(trackState.pattern());
        }

        _trackEngines[trackIndex]->setSwing(_model.project().swing());
    }
}

void Engine::updateTrackSequences() {
    auto &project = _model.project();

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        _trackEngines[trackIndex]->setPattern(project.playState().trackState(trackIndex).pattern());
    }
}

void Engine::updateTrackOutputs() {
    int trackGateIndex[CONFIG_TRACK_COUNT];
    int trackCvIndex[CONFIG_TRACK_COUNT];

    for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
        trackGateIndex[i] = 0;
        trackCvIndex[i] = 0;
    }

    const auto &gateOutputTracks = _model.project().gateOutputTracks();
    const auto &cvOutputTracks = _model.project().cvOutputTracks();

    for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
        int gateOutputTrack = gateOutputTracks[i];
        _gateOutput.setGate(i, _trackEngines[gateOutputTrack]->gateOutput(trackGateIndex[gateOutputTrack]++));
        int cvOutputTrack = cvOutputTracks[i];
        _cvOutput.setChannel(i, _trackEngines[cvOutputTrack]->cvOutput(trackCvIndex[cvOutputTrack]++));
    }
}

void Engine::resetTrackEngines() {
    for (auto trackEngine : _trackEngines) {
        trackEngine->reset();
    }
}

void Engine::updatePlayState() {
    auto &playState = _model.project().playState();

    bool hasImmediateRequests = playState.hasImmediateRequests();
    bool hasScheduledRequests = playState.hasScheduledRequests();

    if (!(hasImmediateRequests || hasScheduledRequests)) {
        return;
    }

    uint32_t measureDivisor = (_model.project().syncMeasure() * CONFIG_PPQN * 4);
    bool handleScheduledRequests = (_tick % measureDivisor == 0 || _tick % measureDivisor == measureDivisor - 1);

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        auto &trackState = playState.trackState(trackIndex);
        auto trackEngine = _trackEngines[trackIndex];

        // handle mute requests
        if (
            (trackState.hasRequests(PlayState::TrackState::ImmediateMuteRequest)) ||
            (handleScheduledRequests && trackState.hasRequests(PlayState::TrackState::ScheduledMuteRequest))
        ) {
            trackState.setMute(trackState.requestedMute());
        }

        // handle pattern requests
        if (
            (trackState.hasRequests(PlayState::TrackState::ImmediatePatternRequest)) ||
            (handleScheduledRequests && trackState.hasRequests(PlayState::TrackState::ScheduledPatternRequest))
        ) {
            trackState.setPattern(trackState.requestedPattern());
        }

        // clear requests
        trackState.clearRequests(PlayState::TrackState::ImmediateRequests);
        if (handleScheduledRequests) {
            trackState.clearRequests(PlayState::TrackState::ScheduledRequests);
        }

        // update track engine
        trackEngine->setMute(trackState.mute());
        trackEngine->setFill(trackState.fill());
        trackEngine->setPattern(trackState.pattern());
    }

    playState.clearImmediateRequests();
    if (handleScheduledRequests) {
        playState.clearScheduledRequests();
    }
}

void Engine::receiveMidi() {
    MidiMessage message;
    while (_midi.recv(&message)) {
        receiveMidi(MidiPort::Midi, message);
    }
    while (_usbMidi.recv(&message)) {
        receiveMidi(MidiPort::UsbMidi, message);
    }
}

void Engine::receiveMidi(MidiPort port, const MidiMessage &message) {
    _routingEngine.receiveMidi(port, message);

    int channel = message.channel();
    for (auto trackEngine : _trackEngines) {
        trackEngine->receiveMidi(port, channel, message);
    }
}

void Engine::initClockSources() {
    // Configure slaves
    _clock.slaveConfigure(ClockSourceExternal, 16, Clock::SlaveFreeRunning);
    _clock.slaveConfigure(ClockSourceMidi, 24);
    _clock.slaveConfigure(ClockSourceUsbMidi, 24);

    // Forward external clock signals to clock
    _dio.clockInput.setHandler([&] (bool value) {
        if (value) {
            _clock.slaveTick(ClockSourceExternal);
        }
    });
    _dio.resetInput.setHandler([&] (bool value) {
        if (value) {
            _clock.slaveReset(ClockSourceExternal);
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

void Engine::initClockOutputs() {
    _clock.outputMidi([this] (uint8_t msg) {
        // TODO we should send a single byte with priority
        _midi.send(MidiMessage(msg));
    });

    _clock.outputClock(
        [this] (bool value) {
            _dio.clockOutput.set(value);
        },
        [this] (bool value) {
            _dio.resetOutput.set(value);
        }
    );

}
