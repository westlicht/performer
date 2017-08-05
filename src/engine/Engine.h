#pragma once

#include "Clock.h"
#include "Track.h"

#include "model/Model.h"

#include "drivers/ClockTimer.h"
#include "drivers/ADC.h"
#include "drivers/DAC.h"
#include "drivers/GateOutput.h"
#include "drivers/MIDI.h"

#include <array>

class Engine {
public:
    typedef std::array<Track, CONFIG_TRACK_COUNT> TrackArray;

    Engine(Model &model, ClockTimer &clockTimer, ADC &adc, DAC &dac, GateOutput &gateOutput, MIDI &midi);

    void init();
    void update();

    // transport control
    void start();
    void stop();
    void resume();
    bool running() const { return _running; }

    float bpm() { return _clock.bpm(); }

    uint32_t tick() const { return _tick; }

    const Clock &clock() const { return _clock; }
          Clock &clock()       { return _clock; }

    const TrackArray &tracks() const { return _tracks; }
          TrackArray &tracks()       { return _tracks; }

    const Track &track(int index) const { return _tracks[index]; }
          Track &track(int index)       { return _tracks[index]; }

private:
    Model &_model;
    ADC &_adc;
    DAC &_dac;
    GateOutput &_gateOutput;
    MIDI &_midi;

    Clock _clock;
    TrackArray _tracks;

    bool _running = false;
    uint32_t _tick = 0;
};
