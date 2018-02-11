#include "Engine.h"

#include "Config.h"

#include "core/Debug.h"
#include "core/midi/MIDIMessage.h"

Engine::Engine(Model &model, ClockTimer &clockTimer, ADC &adc, DAC &dac, DIO &dio, GateOutput &gateOutput, MIDI &midi, USBMIDI &usbMidi) :
    _model(model),
    _adc(adc),
    _dac(dac),
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

    setupClockSources();
    setupClockOutputs();

    for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
        _tracks[i].setSequence(_model.project().pattern(0).sequence(i));
    }
}

void Engine::update() {
    if (_clock.checkStart()) {
        DBG("START");
        for (auto &track : _tracks) {
            track.reset();
        }
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

    // update tempo
    _clock.setMasterBpm(_model.project().bpm());

    _cvInput.update();

    uint32_t tick;
    while (_clock.checkTick(&tick)) {
        _tick = tick;

        for (size_t i = 0; i < _tracks.size(); ++i) {
            auto &track = _tracks[i];
            track.tick(tick);
            _gateOutput.setGate(i, track.gateOutput());
            _cvOutput.setChannel(i, track.cv());
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

void Engine::showMessage(const char *text, uint32_t duration) {
    if (_messageHandler) {
        _messageHandler(text, duration);
    }
}

void Engine::setMessageHandler(MessageHandler handler) {
    _messageHandler = handler;
}

void Engine::setupClockSources() {
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

void Engine::setupClockOutputs() {
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
