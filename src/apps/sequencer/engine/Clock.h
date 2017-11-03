#pragma once

#include "Config.h"

#include <array>
#include <functional>

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

    // Clock output
    void outputConfigure(int ppqn);
    void outputClock(std::function<void()> tick, std::function<void()> reset);
    void outputMIDI(std::function<void(uint8_t)> midi);

    // Sequencer interface
    bool checkStart();
    bool checkStop();
    bool checkResume();
    bool checkTick(uint32_t *tick);

private:
    void resetTicks();
    void requestStart();
    void requestStop();
    void requestResume();

    void setupMasterTimer();
    void setupSlaveTimer();

    void outputMIDIMessage(uint8_t msg);
    void outputTick(uint32_t tick);

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

    struct Output {
        int ppqn;
        std::function<void(uint8_t)> midi;
    };
    Output _output;

    uint8_t _requestStart = 0;
    uint8_t _requestStop = 0;
    uint8_t _requestResume = 0;

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
