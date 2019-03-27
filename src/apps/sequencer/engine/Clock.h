#pragma once

#include "Config.h"

#include "core/utils/MovingAverage.h"

#include "drivers/ClockTimer.h"

#include <array>
#include <functional>

#include <cstdint>

class Clock : private ClockTimer::Listener {
public:
    enum class Mode {
        Auto,
        Master,
        Slave,
    };

    enum Event {
        Start       = (1<<0),
        Stop        = (1<<1),
        Continue    = (1<<2),
        Reset       = (1<<3),
    };

    struct OutputState {
        bool clock = false;
        bool reset = true;
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

    bool isIdle() const { return _state == State::Idle; }
    bool isRunning() const { return _state != State::Idle; }

    int ppqn() const { return _ppqn; }
    float bpm() const { return _state == State::SlaveRunning ? _slaveBpm : _masterBpm; }
    uint32_t tick() const { return _tick; }
    float tickDuration() const { return 60.f / (bpm() * _ppqn); }

    // Master clock control
    void masterStart();
    void masterStop();
    void masterContinue();
    void masterReset();

    float masterBpm() const { return _masterBpm; }
    void setMasterBpm(float bpm);

    // Slave clock control
    void slaveConfigure(int slave, int divisor, bool enabled);
    void slaveTick(int slave);
    void slaveStart(int slave);
    void slaveStop(int slave);
    void slaveContinue(int slave);
    void slaveReset(int slave);
    void slaveHandleMidi(int slave, uint8_t msg);

    // Clock output
    void outputConfigure(int divisor, int pulse);
    const OutputState &outputState() const { return _outputState; }

    // Sequencer interface
    Event checkEvent();
    bool checkTick(uint32_t *tick);

private:
    enum class State {
        Idle,
        // Running,
        MasterRunning,
        SlaveRunning,
    };

    // ClockTimer::Listener
    void onClockTimerTick();

    void resetTicks();
    void requestStart();
    void requestStop();
    void requestContinue();
    void requestReset();
    void requestEvent(Event event);

    void setState(State state);

    void setupMasterTimer();
    void setupSlaveTimer();

    void outputMidiMessage(uint8_t msg);
    void outputTick(uint32_t tick);
    void outputClock(bool clock);
    void outputReset(bool reset);
    void outputRun(bool run);

    bool slaveEnabled(int slave) const { return _slaves[slave].enabled; }

    static constexpr uint32_t SlaveTimerPeriod = 100; // us
    static constexpr size_t SlaveCount = 4;

    Listener *_listener = nullptr;

    ClockTimer &_timer;
    int _ppqn = CONFIG_PPQN;

    Mode _mode = Mode::Auto;

    float _masterBpm = 120.f;

    struct Slave {
        int divisor;
        bool enabled;
    };
    std::array<Slave, SlaveCount> _slaves;

    struct Output {
        int divisor;
        int pulse;
    };
    Output _output;
    OutputState _outputState;

    uint32_t _requestedEvents = Reset;
    State _state = State::Idle;

    volatile uint32_t _tick;
    volatile uint32_t _tickProcessed;

    volatile int32_t _activeSlave = -1;

    uint32_t _elapsedUs;
    uint32_t _lastSlaveTickUs; // time of last call to slaveTick
    uint32_t _slaveTickPeriodUs = 0; // slave tick period time
    uint32_t _slaveSubTicksPending; // number of slave sub ticks pending
    uint32_t _slaveSubTickPeriodUs = 0; // slave sub tick period time
    uint32_t _nextSlaveSubTickUs; // time of next slave sub tick

    float _slaveBpmFiltered = 0.f;
    MovingAverage<float, 4> _slaveBpmAvg;
    float _slaveBpm = 0.f;
};
