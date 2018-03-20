#include "Engine.h"

#include "Config.h"

#include "core/Debug.h"
#include "core/midi/MIDIMessage.h"

#include "os/os.h"

Engine::Engine(Model &model, ClockTimer &clockTimer, ADC &adc, DAC &dac, DIO &dio, GateOutput &gateOutput, MIDI &midi, USBMIDI &usbMidi) :
    _model(model),
    _dio(dio),
    _gateOutput(gateOutput),
    _midi(midi),
    _usbMidi(usbMidi),
    _cvInput(adc),
    _cvOutput(dac),
    _clock(clockTimer),
    _controllerManager(usbMidi)
{
    _cvOutputOverrideValues.fill(0.f);
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

    receiveMIDI();

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

    uint32_t tick;
    while (_clock.checkTick(&tick)) {
        _tick = tick;

        // update play state
        updatePlayState();

        for (size_t i = 0; i < _trackEngines.size(); ++i) {
            auto &trackEngine = _trackEngines[i];
            trackEngine.setSwing(_model.project().swing());
            trackEngine.tick(tick);
            _gateOutput.setGate(i, trackEngine.gateOutput());
            _cvOutput.setChannel(i, trackEngine.cvOutput());
        }
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
        const auto &trackSetup = _model.project().trackSetup(trackIndex);
        int linkTrack = trackSetup.linkTrack();
        const TrackEngine *linkedTrackEngine = linkTrack >= 0 ? &trackEngine(linkTrack) : nullptr;
        trackEngine(trackIndex).setup(trackSetup, linkedTrackEngine);
    }
}

void Engine::updateTrackSequences() {
    auto &project = _model.project();

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        _trackEngines[trackIndex].setSequence(project.sequence(trackIndex, project.playState().trackState(trackIndex).pattern()));
    }
}

void Engine::resetTrackEngines() {
    for (auto &trackEngine : _trackEngines) {
        trackEngine.reset();
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
        auto &trackEngine = _trackEngines[trackIndex];

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
        trackEngine.setMute(trackState.mute());
        trackEngine.setFill(trackState.fill());
        trackEngine.setSequence(_model.project().sequence(trackIndex, trackState.pattern()));
    }

    playState.clearImmediateRequests();
    if (handleScheduledRequests) {
        playState.clearScheduledRequests();
    }
}

void Engine::receiveMIDI() {
    MIDIMessage message;
    while (_midi.recv(&message)) {
        receiveMIDI(MIDIPort::MIDI, message);
    }
    while (_usbMidi.recv(&message)) {
        receiveMIDI(MIDIPort::USBMIDI, message);
    }
}

void Engine::receiveMIDI(MIDIPort port, const MIDIMessage &message) {
}

void Engine::initClockSources() {
    // Configure slaves
    _clock.slaveConfigure(ClockSourceExternal, 16, Clock::SlaveFreeRunning);
    _clock.slaveConfigure(ClockSourceMIDI, 24);
    _clock.slaveConfigure(ClockSourceUSBMIDI, 24);

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
        if (MIDIMessage::isClockMessage(data)) {
            _clock.slaveHandleMIDI(ClockSourceMIDI, data);
            return true;
        }
        return false;
    });
    _usbMidi.setRecvFilter([this] (uint8_t data) {
        if (MIDIMessage::isClockMessage(data)) {
            _clock.slaveHandleMIDI(ClockSourceUSBMIDI, data);
            return true;
        }
        return false;
    });
}

void Engine::initClockOutputs() {
    _clock.outputMIDI([this] (uint8_t msg) {
        // TODO we should send a single byte with priority
        _midi.send(MIDIMessage(msg));
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
