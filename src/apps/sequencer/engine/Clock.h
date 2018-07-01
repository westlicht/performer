#pragma once

#include "Config.h"

#include "core/utils/MovingAverage.h"

#include "drivers/ClockTimer.h"

#include <array>
#include <functional>

#include <cstdint>

class Clock : public ClockTimer::Listener {
public:
    enum Mode {
        ModeAuto,
        ModeMaster,
        ModeSlave,
    };

    enum SlaveFlags {
        SlaveEnabled        = (1<<0),
        SlaveFreeRunning    = (1<<1),
    };

    struct OutputState {
        bool clock = false;
        bool reset = false;
        bool run = false;
    };

    struct Listener {
        virtual void onClockOutput(const OutputState &state) = 0;
        virtual void onClockMidi(uint8_t) = 0;
    };

    Clock(ClockTimer &timer);

    void setListener(Listener *listener);

    void init();

    Mode mode() const { return _mode; }
    void setMode(Mode mode);

    Mode activeMode() const;

    bool isIdle() const { return _state == Idle; }
    bool isRunning() const { return _state != Idle; }

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
    void slaveConfigure(int slave, int divisor, int flags = 0);
    void slaveTick(int slave);
    void slaveStart(int slave);
    void slaveStop(int slave);
    void slaveResume(int slave);
    void slaveReset(int slave);
    void slaveHandleMidi(int slave, uint8_t msg);

    // Clock output
    void outputConfigure(int divisor, int pulse);
    const OutputState &outputState() const { return _outputState; }

    // Sequencer interface
    bool checkStart();
    bool checkStop();
    bool checkResume();
    bool checkTick(uint32_t *tick);

    // ClockTimer::Listener
    void onClockTimerTick();

private:
    enum State {
        Idle,
        // Running,
        MasterRunning,
        SlaveRunning,
    };

    void resetTicks();
    void requestStart();
    void requestStop();
    void requestResume();

    void setState(State state);

    void setupMasterTimer();
    void setupSlaveTimer();

    void outputMidiMessage(uint8_t msg);
    void outputTick(uint32_t tick);
    void outputClock(bool clock);
    void outputReset(bool reset);
    void outputRun(bool run);

    bool slaveEnabled(int slave) const { return _slaves[slave].flags & SlaveEnabled; }

    static constexpr size_t SlaveCount = 4;

    Listener *_listener = nullptr;

    ClockTimer &_timer;
    int _ppqn = CONFIG_PPQN;

    Mode _mode = ModeAuto;

    float _masterBpm = 120.f;

    struct Slave {
        int divisor;
        int flags;
    };
    std::array<Slave, SlaveCount> _slaves;

    struct Output {
        int divisor;
        int pulse;
    };
    Output _output;
    OutputState _outputState;

    uint8_t _requestStart = 0;
    uint8_t _requestStop = 0;
    uint8_t _requestResume = 0;

    State _state = Idle;

    volatile uint32_t _tick;
    volatile uint32_t _tickProcessed;

    volatile int32_t _activeSlave = -1;

    volatile uint32_t _elapsedUs;
    volatile uint32_t _lastTickUs;

    float _slaveBpmFiltered = 0.f;
    MovingAverage<float, 4> _slaveBpmAvg;
    float _slaveBpm = 0.f;
};
