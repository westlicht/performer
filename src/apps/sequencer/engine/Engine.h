#pragma once

#include "EngineState.h"
#include "Clock.h"
#include "TapTempo.h"
#include "NudgeTempo.h"
#include "TrackEngine.h"
#include "NoteTrackEngine.h"
#include "CurveTrackEngine.h"
#include "MidiCvTrackEngine.h"
#include "CvInput.h"
#include "CvOutput.h"
#include "RoutingEngine.h"
#include "MidiOutputEngine.h"
#include "MidiPort.h"
#include "MidiLearn.h"
#include "CvGateToMidiConverter.h"
#include "UpdateReducer.h"

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

class Engine : private Clock::Listener {
public:
    typedef Container<NoteTrackEngine, CurveTrackEngine, MidiCvTrackEngine> TrackEngineContainer;
    typedef std::array<TrackEngineContainer, CONFIG_TRACK_COUNT> TrackEngineContainerArray;
    typedef std::array<TrackEngine *, CONFIG_TRACK_COUNT> TrackEngineArray;
    typedef std::array<UpdateReducer<os::time::ms(25)>, CONFIG_TRACK_COUNT> TrackUpdateReducerArray;

    typedef std::function<bool(MidiPort port, uint8_t cable, const MidiMessage &message)> MidiReceiveHandler;

    typedef std::function<void(uint16_t vendorId, uint16_t productId)> UsbMidiConnectHandler;
    typedef std::function<void()> UsbMidiDisconnectHandler;

    typedef std::function<void(const char *text, uint32_t duration)> MessageHandler;

    enum ClockSource {
        ClockSourceExternal,
        ClockSourceMidi,
        ClockSourceUsbMidi,
    };

    struct Stats {
        uint32_t uptime;
        uint32_t midiRxOverflow;
        uint32_t usbMidiRxOverflow;
    };

    Engine(Model &model, ClockTimer &clockTimer, Adc &adc, Dac &dac, Dio &dio, GateOutput &gateOutput, Midi &midi, UsbMidi &usbMidi);

    void init();
    void update();

    // locking temporarily puts the engine in a state where completely skips all updates
    // lock should only be hold for very short amounts of time
    void lock();
    void unlock();
    bool isLocked() const { return _locked; }

    // suspending temporarily puts the engine in a state where it only processes basic events but skips all updates
    // suspending can be used during longer periods of time (e.g. file operations)
    void suspend();
    void resume();
    bool isSuspended() const { return _suspended; }

    // clock control
    void togglePlay(bool shift = false);
    void clockStart();
    void clockStop();
    void clockContinue();
    void clockReset();
    bool clockRunning() const;

    // recording
    void toggleRecording();
    void setRecording(bool recording);
    bool recording() const;

    // tempo
    float tempo() const { return _clock.bpm(); }

    // tap tempo
    void tapTempoReset();
    void tapTempoTap();

    // nudge tempo
    void nudgeTempoSetDirection(int direction);
    float nudgeTempoStrength() const;

    // time base
    uint32_t tick() const { return _tick; }
    uint32_t noteDivisor() const;
    uint32_t measureDivisor() const;
    float measureFraction() const;
    uint32_t syncDivisor() const;
    float syncFraction() const;

    const CvInput &cvInput() const { return _cvInput; }
    const CvOutput &cvOutput() const { return _cvOutput; }
    const uint8_t gateOutput() const { return _gateOutput.gates(); }

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

    const EngineState &state() const { return _state; }

    const TrackEngineArray &trackEngines() const { return _trackEngines; }
          TrackEngineArray &trackEngines()       { return _trackEngines; }

    const TrackEngine &trackEngine(int index) const { return *_trackEngines[index]; }
          TrackEngine &trackEngine(int index)       { return *_trackEngines[index]; }

    const TrackEngine &selectedTrackEngine() const { return *_trackEngines[_model.project().selectedTrackIndex()]; }
          TrackEngine &selectedTrackEngine()       { return *_trackEngines[_model.project().selectedTrackIndex()]; }

    const RoutingEngine &routingEngine() const { return _routingEngine; }
          RoutingEngine &routingEngine()       { return _routingEngine; }

    const MidiOutputEngine &midiOutputEngine() const { return _midiOutputEngine; }
          MidiOutputEngine &midiOutputEngine()       { return _midiOutputEngine; }

    const MidiLearn &midiLearn() const { return _midiLearn; }
          MidiLearn &midiLearn()       { return _midiLearn; }

    bool trackEnginesConsistent() const;
    bool trackPatternsConsistent() const;

    bool sendMidi(MidiPort port, uint8_t cable, const MidiMessage &message);
    void setMidiReceiveHandler(MidiReceiveHandler handler) { _midiReceiveHandler = handler; }
    void setUsbMidiConnectHandler(UsbMidiConnectHandler handler) { _usbMidiConnectHandler = handler; }
    void setUsbMidiDisconnectHandler(UsbMidiDisconnectHandler handler) { _usbMidiDisconnectHandler = handler; }
    bool midiProgramChangesEnabled();
    void sendMidiProgramChange(int programNumber);
    void sendMidiProgramSave(int programNumber);

    // message handling
    void showMessage(const char *text, uint32_t duration = 1000);
    void setMessageHandler(MessageHandler handler);

    Stats stats() const;

private:
    // Clock::Listener
    virtual void onClockOutput(const Clock::OutputState &state) override;
    virtual void onClockMidi(uint8_t data) override;

    void updateTrackSetups();
    void updateTrackOutputs();
    void reset();
    void updatePlayState(bool ticked);
    void updateOverrides();

    void usbMidiConnect(uint16_t vendorId, uint16_t productId);
    void usbMidiDisconnect();

    void receiveMidi();
    void receiveMidi(MidiPort port, uint8_t cable, const MidiMessage &message);
    void monitorMidi(const MidiMessage &message);

    void initClock();
    void updateClockSetup();

    Model &_model;
    Project &_project;
    Dio &_dio;
    GateOutput &_gateOutput;
    Midi &_midi;
    UsbMidi &_usbMidi;

    EngineState _state;

    CvInput _cvInput;
    CvOutput _cvOutput;

    Clock _clock;
    TapTempo _tapTempo;
    NudgeTempo _nudgeTempo;

    TrackEngineContainerArray _trackEngineContainers;
    TrackEngineArray _trackEngines;
    TrackUpdateReducerArray _trackUpdateReducers;

    MidiOutputEngine _midiOutputEngine;

    RoutingEngine _routingEngine;
    MidiLearn _midiLearn;
    MidiReceiveHandler _midiReceiveHandler;
    UsbMidiConnectHandler _usbMidiConnectHandler;
    UsbMidiDisconnectHandler _usbMidiDisconnectHandler;

    CvGateToMidiConverter _cvGateToMidiConverter;

    // locking
    volatile uint32_t _requestLock = 0;
    volatile uint32_t _locked = 0;

    // suspending
    volatile uint32_t _requestSuspend = 0;
    volatile uint32_t _suspended = 0;

    uint32_t _tick = 0;

    uint32_t _lastSystemTicks = 0;

    // midi monitoring
    struct {
        Types::MidiInputMode lastMidiInputMode;
        MidiSourceConfig lastMidiInputSource;
        Types::CvGateInput lastCvGateInput;

        bool inputChanged(const Project &project) {
            bool changed =
                project.midiInputMode() != lastMidiInputMode ||
                project.midiInputSource() != lastMidiInputSource ||
                project.cvGateInput() != lastCvGateInput;
            if (changed) {
                lastMidiInputMode = project.midiInputMode();
                lastMidiInputSource = project.midiInputSource();
                lastCvGateInput = project.cvGateInput();
            }
            return changed;
        }

        int8_t lastNote = -1;
        int8_t lastTrack = -1;
    } _midiMonitoring;

    // TODO Could be a setting if needed
    static const bool _preSendMidiPgmChange = true;
    bool _midiHasSentInitialPgmChange = false;
    int _midiLastInitialProgramOffset = -1;
    // State machine for when to pre-handle (midi) events
    // Allows us to handle pre-handle events even if they are submitted after the pre-handle tick
    // (then we process them immediately)
    enum PreHandle {
        PreHandleNone,
        PreHandlePending,
        PreHandleComplete,
    };
    PreHandle _pendingPreHandle = PreHandleNone;

    // gate output overrides
    bool _gateOutputOverride = false;
    uint8_t _gateOutputOverrideValue = 0;

    // cv output overrides
    bool _cvOutputOverride = false;
    std::array<float, CvOutput::Channels> _cvOutputOverrideValues;

    MessageHandler _messageHandler;
};
