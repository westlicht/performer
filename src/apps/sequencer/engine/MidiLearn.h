#pragma once

#include "MidiPort.h"

#include "core/midi/MidiMessage.h"

#include <array>
#include <functional>

class MidiLearn {
public:
    enum class Controller : uint8_t {
        ControlAbsolute,
        ControlRelative,
        PitchBend,
        Note,
        Last,
    };

    struct Result {
        MidiPort port;
        uint8_t channel;
        Controller controller;
        union {
            uint8_t controlNumber;
            uint8_t note;
        };

        bool operator==(const Result &other) const {
            return
                port == other.port &&
                channel == other.channel &&
                controller == other.controller &&
                controlNumber == other.controlNumber;
        }
    };

    typedef std::function<void(const Result &result)> ResultCallback;

    MidiLearn();

    void start(ResultCallback callback);
    void stop();

    void receiveMidi(MidiPort port, const MidiMessage &message);

private:
    void reset();
    void emitResult(Result result);

    ResultCallback _callback;

    MidiPort _port;
    int8_t _channel;
    int8_t _controlNumber;
    int8_t _note;

    Result _lastResult;

    std::array<uint8_t, size_t(Controller::Last)> _messageCounters;
};
