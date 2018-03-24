#pragma once

#include "Config.h"
#include "NoteSequence.h"
#include "Serialize.h"

class MidiCvTrack {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    //----------------------------------------
    // Properties
    //----------------------------------------

    // port

    Types::MidiPort port() const { return _port; }
    void setPort(Types::MidiPort port) { _port = port; }

    void editPort(int value, bool shift) {
        setPort(ModelUtils::adjustedEnum(port(), value));
    }

    void printPort(StringBuilder &str) const {
        str(Types::midiPortName(_port));
    }

    // channel

    int channel() const { return _channel; }
    void setChannel(int channel) { _channel = clamp(channel, -1, 15); }

    void editChannel(int value, bool shift) {
        setChannel(channel() + value);
    }

    void printChannel(StringBuilder &str) const {
        Types::printMidiChannel(str, _channel);
    }

    // voices

    int voices() const { return _voices; }
    void setVoices(int voices) { _voices = clamp(voices, 1, 8); }

    void editVoices(int value, bool shift) {
        setVoices(voices() + value);
    }

    void printVoices(StringBuilder &str) const {
        str("%d", voices());
    }

    // outputVelocity

    bool outputVelocity() const { return _outputVelocity; }
    void setOutputVelocity(bool outputVelocity) { _outputVelocity = outputVelocity; }

    void editOutputVelocity(int value, bool shift) {
        setOutputVelocity(value > 0);
    }

    void printOutputVelocity(StringBuilder &str) const {
        return ModelUtils::printYesNo(str, _outputVelocity);
    }

    // outputPressure

    bool outputPressure() const { return _outputPressure; }
    void setOutputPressure(bool outputPressure) { _outputPressure = outputPressure; }

    void editOutputPressure(int value, bool shift) {
        setOutputPressure(value > 0);
    }

    void printOutputPressure(StringBuilder &str) const {
        return ModelUtils::printYesNo(str, _outputPressure);
    }

    // legato

    // glide

    // retrigger

    //----------------------------------------
    // Methods
    //----------------------------------------

    MidiCvTrack() { clear(); }

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    Types::MidiPort _port;
    int8_t _channel;
    uint8_t _voices;
    bool _outputVelocity;
    bool _outputPressure;
};
