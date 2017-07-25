#pragma once

#include "Clock.h"
#include "Track.h"

#include "drivers/ClockTimer.h"
#include "drivers/ADC.h"
#include "drivers/DAC.h"
#include "drivers/Gate.h"
#include "drivers/MIDI.h"

class Engine {
public:
    Engine(ClockTimer &clockTimer, ADC &adc, DAC &dac, Gate &gate, MIDI &midi);

    void init();
    void update();

    // transport control
    void start();
    void stop();
    void resume();
    bool running() const { return _running; }

    uint32_t tick() const { return _tick; }

    const Clock &clock() const { return _clock; }
          Clock &clock()       { return _clock; }

    const Track::Array &tracks() const { return _tracks; }
          Track::Array &tracks()       { return _tracks; }

    const Track &track(int index) const { return _tracks[index]; }
          Track &track(int index)       { return _tracks[index]; }

private:
    ADC &_adc;
    DAC &_dac;
    Gate &_gate;
    MIDI &_midi;

    Clock _clock;
    Track::Array _tracks;

    bool _running = false;
    uint32_t _tick = 0;
};
