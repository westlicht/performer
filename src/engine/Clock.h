#pragma once

#include "Config.h"

#include <array>

#include <cstdint>

class ClockTimer;

class Clock {
public:
    enum Mode {
        ModeAuto,
        ModeMaster,
        ModeSlave,
    };

    enum SlaveFlags {
        SlaveFreeRunning = (1<<0),
    };

    Clock(ClockTimer &timer);

    void init();

    Mode mode() const { return _mode; }
    void setMode(Mode mode);

    int ppqn() const { return _ppqn; }
    float bpm() const { return _state == SlaveRunning ? _slaveBpm : _masterBpm; }
    uint32_t tick() const { return _tick; }

    // Master clock control
    void masterStart();
    void masterStop();
    void masterResume();

    float masterBpm() const { return _masterBpm; }
    void setMasterBpm(float bpm);

    // Slave clock control
    void slaveConfigure(int slave, int ppqn, int flags = 0);
    void slaveTick(int slave);
    void slaveStart(int slave);
    void slaveStop(int slave);
    void slaveResume(int slave);
    void slaveReset(int slave);
    void slaveHandleMIDI(int slave, uint8_t msg);

    // Sequencer interface
    bool checkStart();
    bool checkStop();
    bool checkResume();
    bool checkTick(uint32_t *tick);

private:
    void resetTicks();
    void requestStart() { _requestStart = 1; }
    void requestStop() { _requestStop = 1; }
    void requestResume() { _requestResume = 1; }

    void setupMasterTimer();
    void setupSlaveTimer();

    static constexpr size_t SlaveCount = 4;

    ClockTimer &_timer;
    int _ppqn = CONFIG_PPQN;

    Mode _mode = ModeAuto;

    float _masterBpm = 120.f;

    struct Slave {
        int ppqn;
        int flags;
    };
    std::array<Slave, SlaveCount> _slaves;

    uint8_t _requestStart;
    uint8_t _requestStop;
    uint8_t _requestResume;

    enum State {
        Idle,
        Running,
        MasterRunning,
        SlaveRunning,
    };

    State _state = Idle;

    volatile uint32_t _tick;
    volatile uint32_t _tickProcessed;

    volatile int32_t _activeSlave = -1;

    volatile uint32_t _elapsedUs;
    volatile uint32_t _lastTickUs;

    float _slaveBpm = 0.f;
};
