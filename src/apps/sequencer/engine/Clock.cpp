#include "Clock.h"

#include "os/os.h"
#include "core/Debug.h"
#include "core/midi/MidiMessage.h"
#include "drivers/ClockTimer.h"

#include <cmath>

Clock::Clock(ClockTimer &timer) :
    _timer(timer)
{
    resetTicks();
}

void Clock::init() {
    _timer.disable();
}

void Clock::setMode(Mode mode) {
    if (mode != _mode) {
        if (mode == ModeMaster && _state == SlaveRunning) {
            slaveStop(_activeSlave);
        }
        if (mode == ModeSlave && _state == MasterRunning) {
            masterStop();
        }
        _mode = mode;
    }
}

Clock::Mode Clock::activeMode() {
    switch (_state) {
    case MasterRunning: return ModeMaster;
    case SlaveRunning:  return ModeSlave;
    default:            return _mode;
    }
}

void Clock::masterStart() {
    os::InterruptLock lock;

    if (_state == SlaveRunning || _mode == ModeSlave) {
        return;
    }

    _state = MasterRunning;
    requestStart();
    resetTicks();

    _timer.disable();
    setupMasterTimer();
    _timer.enable();
}

void Clock::masterStop() {
    os::InterruptLock lock;

    if (_state != MasterRunning) {
        return;
    }

    _state = Idle;
    requestStop();

    _timer.disable();
}

void Clock::masterResume() {
    os::InterruptLock lock;

    if (_state != Idle || _mode == ModeSlave) {
        return;
    }

    _state = MasterRunning;
    requestResume();

    _timer.disable();
    setupMasterTimer();
    _timer.enable();
}

void Clock::setMasterBpm(float bpm) {
    os::InterruptLock lock;

    _masterBpm = bpm;
    if (_state == MasterRunning) {
        setupMasterTimer();
    }
}

void Clock::slaveConfigure(int slave, int ppqn, int flags) {
    _slaves[slave] = { ppqn, flags };
}

void Clock::slaveTick(int slave) {
    // free running slaves start the clock with the first pulse
    if ((_slaves[slave].flags & SlaveFreeRunning) && _state == Idle && _mode != ModeMaster) {
        slaveStart(slave);
    }

    {
        os::InterruptLock lock;
        if (_state == SlaveRunning && _activeSlave == slave) {
            // _tick += _ppqn / _slaves[slave].ppqn;
            for (int i = 0; i < _ppqn / _slaves[slave].ppqn; ++i) {
                outputTick(_tick);
                ++_tick;
            }

            // estimate BPM
            uint32_t tickUs = _elapsedUs - _lastTickUs;

            if (tickUs > 0 && _lastTickUs > 0) {
                float bpm = (60.f * 1000000) / (tickUs * _slaves[slave].ppqn);
                _slaveBpmFiltered = 0.9f * _slaveBpmFiltered + 0.1f * bpm;
                // if (std::abs(bpm - _slaveBpm) > 5.f) {
                //     _slaveBpm = 0.5f * _slaveBpm + 0.5f * bpm;
                // }
                _slaveBpmAvg.push(_slaveBpmFiltered);
                _slaveBpm = _slaveBpmAvg();
            }

            _lastTickUs = _elapsedUs;
        }
    }
}

void Clock::slaveStart(int slave) {
    os::InterruptLock lock;

    if (_state == MasterRunning || _mode == ModeMaster || (_state == SlaveRunning && _activeSlave != slave)) {
        return;
    }

    _state = SlaveRunning;
    _activeSlave = slave;
    requestStart();

    resetTicks();

    _timer.disable();
    setupSlaveTimer();
    _timer.enable();
}

void Clock::slaveStop(int slave) {
    os::InterruptLock lock;

    if (_state != SlaveRunning || _mode == ModeMaster || _activeSlave != slave) {
        return;
    }

    _state = Idle;
    _activeSlave = -1;
    requestStop();

    _timer.disable();
}

void Clock::slaveResume(int slave) {
    os::InterruptLock lock;

    if (_state != Idle || _mode == ModeMaster) {
        return;
    }

    _state = SlaveRunning;
    _activeSlave = slave;
    requestResume();

    setupSlaveTimer();
    _timer.enable();
}

void Clock::slaveReset(int slave) {
    os::InterruptLock lock;

    if (_state == MasterRunning || _mode == ModeMaster || (_state == SlaveRunning && _activeSlave != slave)) {
        return;
    }

    _state = Idle;
    _activeSlave = -1;
    requestStart();
    requestStop();

    _timer.disable();
}

void Clock::slaveHandleMidi(int slave, uint8_t msg) {
    switch (MidiMessage::realTimeMessage(msg)) {
    case MidiMessage::Tick:
        slaveTick(slave);
        break;
    case MidiMessage::Start:
        slaveStart(slave);
        break;
    case MidiMessage::Stop:
        slaveStop(slave);
        break;
    case MidiMessage::Continue:
        slaveResume(slave);
        break;
    default:
        break;
    }
}

void Clock::outputConfigure(int ppqn) {
    _output.ppqn = ppqn;
}

void Clock::outputClock(std::function<void(bool)> clock, std::function<void(bool)> reset) {
    _output.clock = clock;
    _output.reset = reset;
}

void Clock::outputMidi(std::function<void(uint8_t)> midi) {
    _output.midi = midi;
}

bool Clock::checkStart() {
    os::InterruptLock lock;
    bool result = _requestStart;
    _requestStart = 0;
    return result;
}

bool Clock::checkStop() {
    os::InterruptLock lock;
    bool result = _requestStop;
    _requestStop = 0;
    return result;
}

bool Clock::checkResume() {
    os::InterruptLock lock;
    bool result = _requestResume;
    _requestResume = false;
    return result;
}

bool Clock::checkTick(uint32_t *tick) {
    os::InterruptLock lock;
    if (_tickProcessed < _tick) {
        *tick = _tickProcessed++;
        return true;
    }
    return false;
}

void Clock::resetTicks() {
    _tick = 0;
    _tickProcessed = 0;
}

void Clock::requestStart() {
    _requestStart = 1;
    outputMidiMessage(MidiMessage::Start);
}

void Clock::requestStop() {
    _requestStop = 1;
    outputMidiMessage(MidiMessage::Stop);
}

void Clock::requestResume() {
    _requestResume = 1;
    outputMidiMessage(MidiMessage::Continue);
}

void Clock::setupMasterTimer() {
    uint32_t us = (60 * 1000000) / (_masterBpm * _ppqn);
    _timer.setPeriod(us);
    _timer.setHandler([&] () { outputTick(_tick); ++_tick; });
}

void Clock::setupSlaveTimer() {
    _elapsedUs = 0;
    _lastTickUs = 0;

    _timer.setPeriod(250);
    _timer.setHandler([&] () { _elapsedUs += 250; });
}

void Clock::outputMidiMessage(uint8_t msg) {
    if (_output.midi) {
        _output.midi(msg);
    }
}

void Clock::outputTick(uint32_t tick) {
    if (tick % (_ppqn / 24) == 0) {
        outputMidiMessage(MidiMessage::Tick);
    }
    if (_output.clock) {
        _output.clock((tick % 2) < 1);
    }
}
