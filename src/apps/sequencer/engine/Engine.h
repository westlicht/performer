#pragma once

#include "Clock.h"
#include "TapTempo.h"
#include "NudgeTempo.h"
#include "TrackEngine.h"
#include "NoteTrackEngine.h"
#include "CurveTrackEngine.h"
#include "MidiCvTrackEngine.h"
#include "Controller.h"
#include "CvInput.h"
#include "CvOutput.h"
#include "RoutingEngine.h"
#include "MidiPort.h"
#include "MidiLearn.h"

#include "model/Model.h"

#include "drivers/ClockTimer.h"
#include "drivers/Adc.h"
#include "drivers/Dac.h"
#include "drivers/Dio.h"
#include "drivers/GateOutput.h"
#include "drivers/Midi.h"
#include "drivers/UsbMidi.h"

#include <array>

#include <cstdint>

class Engine {
public:
    typedef Container<NoteTrackEngine, CurveTrackEngine, MidiCvTrackEngine> TrackEngineContainer;
    typedef std::array<TrackEngineContainer, CONFIG_TRACK_COUNT> TrackEngineContainerArray;
    typedef std::array<TrackEngine *, CONFIG_TRACK_COUNT> TrackEngineArray;

    typedef std::function<void(MidiPort port, const MidiMessage &message)> MidiReceiveHandler;

    typedef std::function<void(const char *text, uint32_t duration)> MessageHandler;

    enum ClockSource {
        ClockSourceExternal,
        ClockSourceMidi,
        ClockSourceUsbMidi,
    };

    Engine(Model &model, ClockTimer &clockTimer, Adc &adc, Dac &dac, Dio &dio, GateOutput &gateOutput, Midi &midi, UsbMidi &usbMidi);

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
    float syncMeasureFraction() const;

    const CvInput &cvInput() const { return _cvInput; }
    const CvOutput &cvOutput() const { return _cvOutput; }

    // gate overrides
    bool gateOutputOverride() const { return _gateOutputOverride; }
    void setGateOutputOverride(bool enabled) { _gateOutputOverride = enabled; }
    void setGateOutput(uint8_t gates) { _gateOutputOverrideValue = gates; }

    // cv overrides
    bool cvOutputOverride() const { return _cvOutputOverride; }
    void setCvOutputOverride(bool enabled) { _cvOutputOverride = enabled; }
    void setCvOutput(int channel, float value) { _cvOutputOverrideValues[channel] = value; }

    const Clock &clock() const { return _clock; }
          Clock &clock()       { return _clock; }

    const TrackEngineArray &tracksEngines() const { return _trackEngines; }
          TrackEngineArray &tracksEngines()       { return _trackEngines; }

    const TrackEngine &trackEngine(int index) const { return *_trackEngines[index]; }
          TrackEngine &trackEngine(int index)       { return *_trackEngines[index]; }

    const TrackEngine &selectedTrackEngine() const { return *_trackEngines[_model.project().selectedTrackIndex()]; }
          TrackEngine &selectedTrackEngine()       { return *_trackEngines[_model.project().selectedTrackIndex()]; }

    const RoutingEngine &routingEngine() const { return _routingEngine; }
          RoutingEngine &routingEngine()       { return _routingEngine; }

    MidiLearn &midiLearn() { return _midiLearn; }

    void setMidiReceiveHandler(MidiReceiveHandler handler) { _midiReceiveHandler = handler; }

    // message handling
    void showMessage(const char *text, uint32_t duration = 1000);
    void setMessageHandler(MessageHandler handler);

private:
    void updateClockSetup();
    void updateTrackSetups();
    void updateTrackSequences();
    void updateTrackOutputs();
    void resetTrackEngines();
    void updatePlayState();

    void receiveMidi();
    void receiveMidi(MidiPort port, const MidiMessage &message);

    void initClockSources();
    void initClockOutputs();

    Model &_model;
    Dio &_dio;
    GateOutput &_gateOutput;
    Midi &_midi;
    UsbMidi &_usbMidi;

    CvInput _cvInput;
    CvOutput _cvOutput;

    Clock _clock;
    TapTempo _tapTempo;
    NudgeTempo _nudgeTempo;

    TrackEngineContainerArray _trackEngineContainers;
    TrackEngineArray _trackEngines;

    RoutingEngine _routingEngine;
    MidiLearn _midiLearn;
    MidiReceiveHandler _midiReceiveHandler;

    ControllerManager _controllerManager;

    bool _running = false;
    uint32_t _tick = 0;

    uint32_t _lastSystemTicks = 0;

    // gate output overrides
    bool _gateOutputOverride = false;
    uint8_t _gateOutputOverrideValue = 0;

    // cv output overrides
    bool _cvOutputOverride = false;
    std::array<float, CvOutput::Channels> _cvOutputOverrideValues;

    MessageHandler _messageHandler;
};
