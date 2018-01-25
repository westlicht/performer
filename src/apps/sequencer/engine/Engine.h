#pragma once

#include "Clock.h"
#include "Track.h"
#include "Controller.h"

#include "model/Model.h"

#include "drivers/ClockTimer.h"
#include "drivers/ADC.h"
#include "drivers/DAC.h"
#include "drivers/DIO.h"
#include "drivers/GateOutput.h"
#include "drivers/MIDI.h"
#include "drivers/USBMIDI.h"

#include <array>

class Engine {
public:
    typedef std::array<Track, CONFIG_TRACK_COUNT> TrackArray;

    enum ClockSource {
        ClockSourceExternal,
        ClockSourceMIDI,
        ClockSourceUSBMIDI,
    };

    Engine(Model &model, ClockTimer &clockTimer, ADC &adc, DAC &dac, DIO &dio, GateOutput &gateOutput, MIDI &midi, USBMIDI &usbMidi);

    void init();
    void update();

    // transport control
    void start();
    void stop();
    void resume();
    bool running() const { return _running; }

    float bpm() { return _clock.bpm(); }

    Clock::Mode clockMode() { return _clock.activeMode(); }

    uint32_t tick() const { return _tick; }

    const Clock &clock() const { return _clock; }
          Clock &clock()       { return _clock; }

    const TrackArray &tracks() const { return _tracks; }
          TrackArray &tracks()       { return _tracks; }

    const Track &track(int index) const { return _tracks[index]; }
          Track &track(int index)       { return _tracks[index]; }

private:
    void setupClockSources();
    void setupClockOutputs();

    Model &_model;
    ADC &_adc;
    DAC &_dac;
    DIO &_dio;
    GateOutput &_gateOutput;
    MIDI &_midi;
    USBMIDI &_usbMidi;

    Clock _clock;
    TrackArray _tracks;

    ControllerManager _controllerManager;

    bool _running = false;
    uint32_t _tick = 0;
};
