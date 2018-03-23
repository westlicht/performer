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

    void printPort(StringBuilder &str) const {
        str(Types::midiPortName(_port));
    }

    // channel

    int channel() const { return _channel; }
    void setChannel(int channel) { _channel = channel; }

    void printChannel(StringBuilder &str) const {
        Types::printMidiChannel(str, _channel);
    }

    // config
    // - 1-voice, gate/pitch
    // - 1-voice, gate/pitch/velocity
    // - 1-voice, gate/pitch/aftertouch
    // - 1-voice, gate/pitch/velocity/aftertouch
    // - 2-voice, gate/pitch
    // - 2-voice, gate/pitch/velocity
    // - 2-voice, gate/pitch/aftertouch
    // - 2-voice, gate/pitch/velocity/aftertouch
    // - 4-voice, gate/pitch
    // - 4-voice, gate/pitch/velocity
    // - 4-voice, gate/pitch/aftertouch
    // - 8-voice, gate/pitch

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
};
