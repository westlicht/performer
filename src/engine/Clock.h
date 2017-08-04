#pragma once

#include "Config.h"

#include "os/Atomic.h"

#include <cstdint>

class ClockTimer;

class Clock {
public:
    enum Mode {
        Auto,
        Master,
        Slave,
    };

    enum State {
        Idle,
        Running,
    };

    Clock(ClockTimer &timer);

    void init();

    Mode mode() const { return _mode; }
    void setMode(Mode mode);

    int ppqn() const { return _ppqn; }
    void setPpqn(int ppqn);

    float bpm() const { return _bpm; }
    void setBpm(float bpm);

    uint32_t tick() const { return _tick; }

    void start();
    void stop();
    void resume();

    bool checkStart();
    bool checkStop();
    bool checkResume();
    bool checkTick(uint32_t *tick);

private:
    void updateTimerPeriod();
    void handleTimer();

    ClockTimer &_timer;
    Mode _mode = Auto;
    State _state = Idle;
    int _ppqn = PPQN;
    float _bpm = 120.f;

    Atomic<uint8_t> _requestStart;
    Atomic<uint8_t> _requestStop;
    Atomic<uint8_t> _requestResume;

    volatile uint32_t _tick = 0;
    volatile uint32_t _tickProcessed = 0;
};
