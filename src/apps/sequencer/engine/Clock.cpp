#include "Clock.h"

#include "os/os.h"
#include "core/Debug.h"
#include "core/math/Math.h"
#include "core/midi/MidiMessage.h"
#include "drivers/ClockTimer.h"

#include <cmath>

Clock::Clock(ClockTimer &timer) :
    _timer(timer)
{
    resetTicks();

    _timer.setListener(this);
}

void Clock::init() {
    _timer.disable();
}

void Clock::setListener(Listener *listener) {
    os::InterruptLock lock;
    _listener = listener;
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

Clock::Mode Clock::activeMode() const {
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

    setState(MasterRunning);
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

    setState(Idle);
    requestStop();

    _timer.disable();
}

void Clock::masterResume() {
    os::InterruptLock lock;

    if (_state != Idle || _mode == ModeSlave) {
        return;
    }

    setState(MasterRunning);
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

void Clock::slaveConfigure(int slave, int divisor, int flags) {
    _slaves[slave] = { divisor, flags };
}

void Clock::slaveTick(int slave) {
    if (!slaveEnabled(slave)) {
        return;
    }

    // free running slaves start the clock with the first pulse
    if ((_slaves[slave].flags & SlaveFreeRunning) && _state == Idle && _mode != ModeMaster) {
        slaveStart(slave);
    }

    {
        os::InterruptLock lock;
        if (_state == SlaveRunning && _activeSlave == slave) {
            int divisor = _slaves[slave].divisor;
            for (int i = 0; i < divisor; ++i) {
                outputTick(_tick);
                ++_tick;
            }

            // estimate BPM
            uint32_t tickUs = _elapsedUs - _lastTickUs;

            if (tickUs > 0 && _lastTickUs > 0) {
                // float bpm = (60.f * 1000000) / (tickUs * (_ppqn / divisor));
                float bpm = (60.f * 1000000 * divisor) / (tickUs * _ppqn);
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
    if (!slaveEnabled(slave)) {
        return;
    }

    os::InterruptLock lock;

    if (_state == MasterRunning || _mode == ModeMaster || (_state == SlaveRunning && _activeSlave != slave)) {
        return;
    }

    setState(SlaveRunning);
    _activeSlave = slave;
    requestStart();

    resetTicks();

    _timer.disable();
    setupSlaveTimer();
    _timer.enable();
}

void Clock::slaveStop(int slave) {
    if (!slaveEnabled(slave)) {
        return;
    }

    os::InterruptLock lock;

    if (_state != SlaveRunning || _mode == ModeMaster || _activeSlave != slave) {
        return;
    }

    setState(Idle);
    _activeSlave = -1;
    requestStop();

    _timer.disable();
}

void Clock::slaveResume(int slave) {
    if (!slaveEnabled(slave)) {
        return;
    }

    os::InterruptLock lock;

    if (_state != Idle || _mode == ModeMaster) {
        return;
    }

    setState(SlaveRunning);
    _activeSlave = slave;
    requestResume();

    setupSlaveTimer();
    _timer.enable();
}

void Clock::slaveReset(int slave) {
    if (!slaveEnabled(slave)) {
        return;
    }

    os::InterruptLock lock;

    if (_state == MasterRunning || _mode == ModeMaster || (_state == SlaveRunning && _activeSlave != slave)) {
        return;
    }

    setState(Idle);
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

void Clock::outputConfigure(int divisor, int pulse) {
    os::InterruptLock lock;
    _output.divisor = divisor;
    _output.pulse = pulse;
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

void Clock::onClockTimerTick() {
    switch (_state) {
    case MasterRunning: {
        outputTick(_tick);
        ++_tick;
        _elapsedUs += _timer.period();
        break;
    }
    case SlaveRunning: {
        _elapsedUs += _timer.period();
        break;
    }
    default:
        break;
    }
}

void Clock::resetTicks() {
    _tick = 0;
    _tickProcessed = 0;
}

void Clock::requestStart() {
    _requestStart = 1;
    outputMidiMessage(MidiMessage::Start);
    outputRun(true);
}

void Clock::requestStop() {
    _requestStop = 1;
    outputMidiMessage(MidiMessage::Stop);
    outputRun(false);
}

void Clock::requestResume() {
    _requestResume = 1;
    outputMidiMessage(MidiMessage::Continue);
    outputRun(true);
}

void Clock::setState(State state) {
    _state = state;
    switch (_state) {
    case Idle:
        outputReset(true);
        break;
    default:
        outputReset(false);
        break;
    }
}

void Clock::setupMasterTimer() {
    _elapsedUs = 0;
    uint32_t us = (60 * 1000000) / (_masterBpm * _ppqn);
    _timer.setPeriod(us);
}

void Clock::setupSlaveTimer() {
    _elapsedUs = 0;
    _lastTickUs = 0;
    _timer.setPeriod(250);
}

void Clock::outputMidiMessage(uint8_t msg) {
    os::InterruptLock lock;
    if (_listener) {
        _listener->onClockMidi(msg);
    }
}

void Clock::outputTick(uint32_t tick) {
    if (tick % (_ppqn / 24) == 0) {
        outputMidiMessage(MidiMessage::Tick);
    }

    uint32_t divisor = _output.divisor;
    uint32_t clockTick = tick % divisor;
    uint32_t clockDuration = clamp(uint32_t(_masterBpm * _ppqn * _output.pulse / (60 * 1000)), uint32_t(1), uint32_t(divisor - 1));
    if (clockTick == 0) {
        outputClock(true);
    } else if (clockTick == clockDuration) {
        outputClock(false);
    }
}

void Clock::outputClock(bool clock) {
    os::InterruptLock lock;
    _outputState.clock = clock;
    if (_listener) {
        _listener->onClockOutput(_outputState);
    }
}

void Clock::outputReset(bool reset) {
    os::InterruptLock lock;
    _outputState.reset = reset;
    if (_listener) {
        _listener->onClockOutput(_outputState);
    }
}

void Clock::outputRun(bool run) {
    os::InterruptLock lock;
    _outputState.run = run;
    if (_listener) {
        _listener->onClockOutput(_outputState);
    }
}
