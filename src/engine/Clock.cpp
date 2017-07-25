#include "Clock.h"

#include "os/os.h"
#include "core/Debug.h"
#include "drivers/ClockTimer.h"

Clock::Clock(ClockTimer &timer) :
    _timer(timer)
{
}

void Clock::init() {
    _timer.setHandler([this] () { handleTimer(); });
}

void Clock::setMode(Mode mode) {
    _mode = mode;
}

void Clock::setPpqn(int ppqn) {
    _ppqn = ppqn;
    updateTimerPeriod();
}

void Clock::setBpm(float bpm) {
    _bpm = bpm;
    updateTimerPeriod();
}

void Clock::start() {
    _requestStart.set(1);
    _timer.disable();
    updateTimerPeriod();
    _tick = 0;
    _tickProcessed = 0;
    _timer.enable();
}

void Clock::stop() {
    _requestStop.set(1);
    _timer.disable();
}

void Clock::resume() {
    _requestResume.set(1);
    _timer.enable();
}

bool Clock::checkStart() {
    return _requestStart.set(0);
}

bool Clock::checkStop() {
    return _requestStop.set(0);
}

bool Clock::checkResume() {
    return _requestResume.set(0);
}

bool Clock::checkTick(uint32_t *tick) {
    os::InterruptLock lock;
    if (_tickProcessed < _tick) {
        *tick = _tickProcessed++;
        return true;
    }
    return false;
}

void Clock::updateTimerPeriod() {
    uint32_t us = (60 * 1000000) / (_bpm * _ppqn);
    _timer.setPeriod(us);
}

void Clock::handleTimer() {
    ++_tick;
}
