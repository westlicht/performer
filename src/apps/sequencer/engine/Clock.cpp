#include "Clock.h"

#include "Groove.h"

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
    if (_listener) {
        _listener->onClockOutput(_outputState);
    }
}

void Clock::setMode(Mode mode) {
    if (mode != _mode) {
        if (mode == Mode::Master && _state == State::SlaveRunning) {
            slaveStop(_activeSlave);
        }
        if (mode == Mode::Slave && _state == State::MasterRunning) {
            masterStop();
        }
        _mode = mode;
    }
}

Clock::Mode Clock::activeMode() const {
    switch (_state) {
    case State::MasterRunning:
        return Mode::Master;
    case State::SlaveRunning:
        return Mode::Slave;
    default:
        return _mode;
    }
}

void Clock::masterStart() {
    os::InterruptLock lock;

    if (_state == State::SlaveRunning || _mode == Mode::Slave) {
        return;
    }

    setState(State::MasterRunning);
    requestStart();
    resetTicks();

    _timer.disable();
    setupMasterTimer();
    _timer.enable();
}

void Clock::masterStop() {
    os::InterruptLock lock;

    if (_state != State::MasterRunning) {
        return;
    }

    setState(State::Idle);
    requestStop();

    _timer.disable();
}

void Clock::masterContinue() {
    os::InterruptLock lock;

    if (_state != State::Idle || _mode == Mode::Slave) {
        return;
    }

    setState(State::MasterRunning);
    requestContinue();

    _timer.disable();
    setupMasterTimer();
    _timer.enable();
}

void Clock::masterReset() {
    os::InterruptLock lock;

    if (_state == State::SlaveRunning || _mode == Mode::Slave) {
        return;
    }

    setState(State::Idle);
    requestReset();

    _timer.disable();
}

void Clock::setMasterBpm(float bpm) {
    os::InterruptLock lock;

    _masterBpm = bpm;
    if (_state == State::MasterRunning) {
        setupMasterTimer();
    }
}

void Clock::slaveConfigure(int slave, int divisor, bool enabled) {
    _slaves[slave] = { divisor, enabled };
}

void Clock::slaveTick(int slave) {
    os::InterruptLock lock;

    if (!slaveEnabled(slave)) {
        return;
    }

    if (_state == State::SlaveRunning && _activeSlave == slave) {
        uint32_t divisor = _slaves[slave].divisor;

        // protect against clock rate overload
        _slaveSubTicksPending = std::min(_slaveSubTicksPending + divisor, 2 * divisor);

        // time past since last tick
        uint32_t periodUs = _elapsedUs - _lastSlaveTickUs;

        // default tick period to 120 bpm
        if (_slaveTickPeriodUs == 0) {
            _slaveTickPeriodUs = (60 * 1000000 * divisor) / (120 * _ppqn);
        }

        // update tick period if we have a valid measurement
        if (periodUs > 0 && _lastSlaveTickUs > 0) {
            _slaveTickPeriodUs = periodUs;
        }

        _slaveSubTickPeriodUs = _slaveTickPeriodUs / _slaveSubTicksPending;
        if (_elapsedUs - _nextSlaveSubTickUs > 1000) {
            _nextSlaveSubTickUs = _elapsedUs;
        } else {
            _nextSlaveSubTickUs += _slaveSubTickPeriodUs;
        }

        // estimate slave BPM
        if (periodUs > 0 && _lastSlaveTickUs > 0) {
            float bpm = (60.f * 1000000 * divisor) / (periodUs * _ppqn);
            _slaveBpmFiltered = 0.9f * _slaveBpmFiltered + 0.1f * bpm;
            _slaveBpmAvg.push(_slaveBpmFiltered);
            _slaveBpm = _slaveBpmAvg();
        }

        _lastSlaveTickUs = _elapsedUs;
    }
}

void Clock::slaveStart(int slave) {
    os::InterruptLock lock;

    if (!slaveEnabled(slave)) {
        return;
    }

    if (_state == State::MasterRunning || _mode == Mode::Master || (_state == State::SlaveRunning && _activeSlave != slave)) {
        return;
    }

    setState(State::SlaveRunning);
    _activeSlave = slave;
    requestStart();

    resetTicks();

    _timer.disable();
    setupSlaveTimer();
    _timer.enable();
}

void Clock::slaveStop(int slave) {
    os::InterruptLock lock;

    if (!slaveEnabled(slave)) {
        return;
    }

    if (_state != State::SlaveRunning || _mode == Mode::Master || _activeSlave != slave) {
        return;
    }

    setState(State::Idle);
    _activeSlave = -1;
    requestStop();

    _timer.disable();
}

void Clock::slaveContinue(int slave) {
    os::InterruptLock lock;

    if (!slaveEnabled(slave)) {
        return;
    }

    if (_state != State::Idle || _mode == Mode::Master) {
        return;
    }

    setState(State::SlaveRunning);
    _activeSlave = slave;
    requestContinue();

    setupSlaveTimer();
    _timer.enable();
}

void Clock::slaveReset(int slave) {
    os::InterruptLock lock;

    if (!slaveEnabled(slave)) {
        return;
    }

    if (_state == State::MasterRunning || _mode == Mode::Master || (_state == State::SlaveRunning && _activeSlave != slave)) {
        return;
    }

    setState(State::Idle);
    _activeSlave = -1;
    requestReset();

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
        slaveContinue(slave);
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

void Clock::outputConfigureSwing(int swing) {
    os::InterruptLock lock;
    _output.swing = swing;
}

#define CHECK(_event_)                  \
    if (_requestedEvents & _event_) {   \
        _requestedEvents &= ~_event_;   \
        return _event_;                 \
    }

Clock::Event Clock::checkEvent() {
    os::InterruptLock lock;

    if (_requestedEvents) {
        CHECK(Start)
        CHECK(Stop)
        CHECK(Continue)
        CHECK(Reset)
    }

    return Event(0);
}

#undef CHECK

bool Clock::checkTick(uint32_t *tick) {
    os::InterruptLock lock;

    if (_requestedEvents) {
        return false;
    }
    if (_tickProcessed < _tick) {
        *tick = _tickProcessed++;
        return true;
    }
    return false;
}

void Clock::onClockTimerTick() {
    os::InterruptLock lock;

    switch (_state) {
    case State::MasterRunning: {
        outputTick(_tick);
        ++_tick;
        _elapsedUs += _timer.period();
        break;
    }
    case State::SlaveRunning: {
        _elapsedUs += _timer.period();

        if (_slaveSubTicksPending > 0 && _elapsedUs >= _nextSlaveSubTickUs) {
            outputTick(_tick);
            ++_tick;
            --_slaveSubTicksPending;
            _nextSlaveSubTickUs += _slaveSubTickPeriodUs;
        }

        if (_mode == Mode::Auto && (_elapsedUs - _lastSlaveTickUs) > 500000) {
            slaveReset(_activeSlave);
        }
        break;
    }
    default:
        break;
    }
}

void Clock::resetTicks() {
    _tick = 0;
    _tickProcessed = 0;
    _slaveSubTicksPending = 0;
    _output.nextTick = 0;
}

void Clock::requestStart() {
    requestEvent(Start);
    outputMidiMessage(MidiMessage::Start);
    // outputMidiMessage(MidiMessage::Tick); // TODO: this seems wrong
    outputRun(true);
    outputReset(true);
}

void Clock::requestStop() {
    requestEvent(Stop);
    outputMidiMessage(MidiMessage::Stop);
    outputRun(false);
    outputReset(false);
}

void Clock::requestContinue() {
    requestEvent(Continue);
    outputMidiMessage(MidiMessage::Continue);
    outputRun(true);
    outputReset(false);
}

void Clock::requestReset() {
    requestEvent(Reset);
    outputMidiMessage(MidiMessage::Stop);
    outputRun(false);
    outputReset(true);
    outputClock(false);
}

void Clock::requestEvent(Event event) {
    _requestedEvents |= event;
}

void Clock::setState(State state) {
    _state = state;
}

void Clock::setupMasterTimer() {
    _elapsedUs = 0;
    uint32_t us = (60 * 1000000) / (_masterBpm * _ppqn);
    _timer.setPeriod(us);
}

void Clock::setupSlaveTimer() {
    _elapsedUs = 0;
    _lastSlaveTickUs = 0;

    _timer.setPeriod(SlaveTimerPeriod);
}

void Clock::outputMidiMessage(uint8_t msg) {
    os::InterruptLock lock;
    if (_listener) {
        _listener->onClockMidi(msg);
    }
}

void Clock::outputTick(uint32_t tick) {
    outputReset(false);

    if (tick % (_ppqn / 24) == 0) {
        outputMidiMessage(MidiMessage::Tick);
    }

    // generate output clock with swing

    auto applySwing = [this] (uint32_t tick) {
        return _output.swing != 0 ? Groove::applySwing(tick, _output.swing) : tick;
    };

    if (tick == _output.nextTick) {
        uint32_t divisor = _output.divisor;
        uint32_t clockDuration = std::max(uint32_t(1), uint32_t(_masterBpm * _ppqn * _output.pulse / (60 * 1000)));

        _output.nextTickOn = applySwing(_output.nextTick);
        _output.nextTickOff = std::min(_output.nextTickOn + clockDuration, applySwing(_output.nextTick + divisor) - 1);

        _output.nextTick += divisor;
    }

    if (tick == _output.nextTickOn) {
        outputClock(true);
    }

    if (tick == _output.nextTickOff) {
        outputClock(false);
    }
}

void Clock::outputClock(bool clock) {
    os::InterruptLock lock;

    if (clock != _outputState.clock) {
        _outputState.clock = clock;
        if (_listener) {
            _listener->onClockOutput(_outputState);
        }
    }
}

void Clock::outputReset(bool reset) {
    os::InterruptLock lock;

    if (reset != _outputState.reset) {
        _outputState.reset = reset;
        if (_listener) {
            _listener->onClockOutput(_outputState);
        }
    }
}

void Clock::outputRun(bool run) {
    os::InterruptLock lock;

    if (run != _outputState.run) {
        _outputState.run = run;
        if (_listener) {
            _listener->onClockOutput(_outputState);
        }
    }
}
