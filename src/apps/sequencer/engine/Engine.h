#pragma once

#include "Clock.h"
#include "TapTempo.h"
#include "NudgeTempo.h"
#include "TrackEngine.h"
#include "Controller.h"
#include "CVInput.h"
#include "CVOutput.h"
#include "MIDIPort.h"

#include "model/Model.h"

#include "drivers/ClockTimer.h"
#include "drivers/ADC.h"
#include "drivers/DAC.h"
#include "drivers/DIO.h"
#include "drivers/GateOutput.h"
#include "drivers/MIDI.h"
#include "drivers/USBMIDI.h"

#include <array>

#include <cstdint>

class Engine {
public:
    typedef std::array<TrackEngine, CONFIG_TRACK_COUNT> TrackEngineArray;

    typedef std::function<void(const char *text, uint32_t duration)> MessageHandler;

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

    // tempo
    float bpm() { return _clock.bpm(); }

    // tap tempo
    void tapTempoReset();
    void tapTempoTap();

    // nudge tempo
    void nudgeTempoSetDirection(int direction);
    float nudgeTempoStrength() const;

    // time base
    uint32_t tick() const { return _tick; }
    float globalMeasureFraction() const;

    const CVInput &cvInput() const { return _cvInput; }
    const CVOutput &cvOutput() const { return _cvOutput; }

    // gate overrides
    bool gateOutputOverride() const { return _gateOutputOverride; }
    void setGateOutputOverride(bool enabled) { _gateOutputOverride = enabled; }
    void setGateOutput(uint8_t gates) { _gateOutputOverrideValue = gates; }

    bool cvOutputOverride() const { return _cvOutputOverride; }
    void setCvOutputOverride(bool enabled) { _cvOutputOverride = enabled; }
    void setCvOutput(int channel, float value) { _cvOutputOverrideValues[channel] = value; }

    const Clock &clock() const { return _clock; }
          Clock &clock()       { return _clock; }

    const TrackEngineArray &tracksEngines() const { return _trackEngines; }
          TrackEngineArray &tracksEngines()       { return _trackEngines; }

    const TrackEngine &trackEngine(int index) const { return _trackEngines[index]; }
          TrackEngine &trackEngine(int index)       { return _trackEngines[index]; }

    const TrackEngine &selectedTrackEngine() const { return _trackEngines[_model.project().selectedTrackIndex()]; }
          TrackEngine &selectedTrackEngine()       { return _trackEngines[_model.project().selectedTrackIndex()]; }

    // message handling
    void showMessage(const char *text, uint32_t duration = 1000);
    void setMessageHandler(MessageHandler handler);

private:
    void updateClockSetup();
    void updateTrackSetups();
    void updateTrackSequences();
    void resetTrackEngines();
    void updatePlayState();

    void receiveMIDI();
    void receiveMIDI(MIDIPort port, const MIDIMessage &message);

    void initClockSources();
    void initClockOutputs();

    Model &_model;
    DIO &_dio;
    GateOutput &_gateOutput;
    MIDI &_midi;
    USBMIDI &_usbMidi;

    CVInput _cvInput;
    CVOutput _cvOutput;

    Clock _clock;
    TapTempo _tapTempo;
    NudgeTempo _nudgeTempo;

    TrackEngineArray _trackEngines;

    ControllerManager _controllerManager;

    bool _running = false;
    uint32_t _tick = 0;
    uint32_t _lastTick = 0;

    uint32_t _lastSystemTicks = 0;

    // gate output overrides
    bool _gateOutputOverride = false;
    uint8_t _gateOutputOverrideValue = 0;

    // cv output overrides
    bool _cvOutputOverride = false;
    std::array<float, CVOutput::Channels> _cvOutputOverrideValues;

    MessageHandler _messageHandler;
};
