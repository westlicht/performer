#pragma once

#include "Config.h"
#include "NoteSequence.h"
#include "Serialize.h"

class MidiCvTrack {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    enum class VoiceConfig {
        Pitch,
        PitchVelocity,
        PitchVelocityPressure,
        Last,
    };

    static const char *voiceConfigName(VoiceConfig voiceConfig) {
        switch (voiceConfig) {
        case VoiceConfig::Pitch:                return "Pitch";
        case VoiceConfig::PitchVelocity:        return "Pitch|Vel";
        case VoiceConfig::PitchVelocityPressure:return "Pitch|Vel|Press";
        case VoiceConfig::Last:                 break;
        }
        return nullptr;
    }

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

    // voiceConfig

    VoiceConfig voiceConfig() const { return _voiceConfig; }
    void setVoiceConfig(VoiceConfig voiceConfig) { _voiceConfig = voiceConfig; }

    void editVoiceConfig(int value, bool shift) {
        setVoiceConfig(ModelUtils::adjustedEnum(voiceConfig(), value));
    }

    void printVoiceConfig(StringBuilder &str) const {
        str(voiceConfigName(_voiceConfig));
    }

    // legato

    // glide

    // retrigger

    //----------------------------------------
    // Methods
    //----------------------------------------

    MidiCvTrack() { clear(); }

    void clear();

    void gateOutputName(int index, StringBuilder &str) const;
    void cvOutputName(int index, StringBuilder &str) const;

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    Types::MidiPort _port;
    int8_t _channel;
    uint8_t _voices;
    VoiceConfig _voiceConfig;
};
