#pragma once

#include <algorithm>

#include <cstdint>

class MIDIMessage {
public:
    // Channel (voice) messages

    enum ChannelMessage {
        NoteOff         = 0x80,
        NoteOn          = 0x90,
        KeyPressure     = 0xa0,
        ControlChange   = 0xb0,
        ProgramChange   = 0xc0,
        ChannelPressure = 0xd0,
        PitchBend       = 0xe0,
    };

    static int channelMessageLength(ChannelMessage message) {
        switch (message) {
        case NoteOff:
        case NoteOn:
        case KeyPressure:
        case ControlChange:
        case PitchBend:
            return 2;
        case ChannelPressure:
        case ProgramChange:
            return 1;
        default:
            return 0;
        }
    }
    static bool isChannelMessage(uint8_t status) { return (status & 0xf0) >= 0x80 && (status & 0xf0) < 0xf0; }

    template<uint8_t ChannelMessage>
    static bool isChannelMessage(uint8_t status) { return (status & ChannelMessage) == ChannelMessage; }

    static ChannelMessage channelMessage(uint8_t status) { return ChannelMessage(status & 0xf0); }

    // System (common) messages

    enum SystemMessage {
        SystemExclusive = 0xf0,
        TimeCode        = 0xf1,
        SongPosition    = 0xf2,
        SongSelect      = 0xf3,
        TuneRequest     = 0xf6,
        EndOfExclusive  = 0xf7,
    };

    static int systemMessageLength(SystemMessage message) {
        switch (message) {
        case TimeCode:
        case SongSelect:
            return 1;
        case SongPosition:
            return 2;
        default:
            return 0;
        }
    }

    static bool isSystemMessage(uint8_t status) { return (status & 0xf8) == 0xf0; }

    template<uint8_t SystemMessage>
    static bool isSystemMessage(uint8_t status) { return status == SystemMessage; }

    static SystemMessage systemMessage(uint8_t status) { return SystemMessage(status); }

    // (System) realtime messages

    enum RealTimeMessage {
        Tick            = 0xf8,
        Start           = 0xfa,
        Continue        = 0xfb,
        Stop            = 0xfc,
        ActiveSensing   = 0xfe,
        Reset           = 0xff,
    };

    static bool isRealTimeMessage(uint8_t status) { return (status & 0xf8) == 0xf8; }

    template<uint8_t RealTimeMessage>
    static bool isRealTimeMessage(uint8_t status) { return status == RealTimeMessage; }

    static RealTimeMessage realTimeMessage(uint8_t status) { return RealTimeMessage(status); }

    static bool isClockMessage(uint8_t status) {
        return isRealTimeMessage(status) && (
            status == Tick ||
            status == Start ||
            status == Continue ||
            status == Stop
        );
    }

    // Message data

    uint8_t status() const { return _status; }
    uint8_t data0() const { return _data[0]; }
    uint8_t data1() const { return _data[1]; }

    // Channel messages

    bool isChannelMessage() const {
        return isChannelMessage(_status);
    }

    ChannelMessage channelMessage() const {
        return MIDIMessage::channelMessage(_status);
    }

    uint8_t channel() const {
        return _status & 0xf;
    }

    // NoteOff

    bool isNoteOff() const {
        return isChannelMessage<NoteOff>(_status);
    }

    // NoteOn

    bool isNoteOn() const {
        return isChannelMessage<NoteOn>(_status);
    }

    uint8_t note() const {
        return _data[0];
    }

    uint8_t velocity() const {
        return _data[1];
    }

    // KeyPressure

    bool isKeyPressure() const {
        return isChannelMessage<KeyPressure>(_status);
    }

    uint8_t keyPressure() const {
        return _data[1];
    }

    // ControlChange

    bool isControlChange() const {
        return isChannelMessage<ControlChange>(_status);
    }

    uint8_t controllerNumber() const {
        return _data[0];
    }

    uint8_t controllerValue() const {
        return _data[1];
    }

    // ProgramChange

    bool isProgramChange() const {
        return isChannelMessage<ProgramChange>(_status);
    }

    uint8_t programNumber() const {
        return _data[0];
    }

    // ChannelPressure

    bool isChannelPressure() const {
        return isChannelMessage<ChannelPressure>(_status);
    }

    uint8_t channelPressure() const {
        return _data[0];
    }

    // PitchBend

    bool isPitchBend() const {
        return isChannelMessage<PitchBend>(_status);
    }

    int pitchBend() const {
        return (int(_data[1]) << 7 | int(_data[0])) - int(0x2000);
    }

    // System messages

    bool isSystemMessage() const {
        return MIDIMessage::isSystemMessage(_status);
    }

    SystemMessage systemMessage() const {
        return MIDIMessage::systemMessage(_status);
    }

    bool isSystemExclusive() const { return isSystemMessage<SystemExclusive>(_status); }
    bool isTimeCode() const { return isSystemMessage<TimeCode>(_status); }
    bool isSongPosition() const { return isSystemMessage<SongPosition>(_status); }
    bool isSongSelect() const { return isSystemMessage<SongSelect>(_status); }
    bool isTuneRequest() const { return isSystemMessage<TuneRequest>(_status); }

    int songPosition() const {
        return (int(_data[1]) << 7 | int(_data[0]));
    }

    uint8_t songNumber() const {
        return _data[0];
    }

    // Real-time messages

    bool isRealTimeMessage() const {
        return MIDIMessage::isRealTimeMessage(_status);
    }

    RealTimeMessage realTimeMessage() const {
        return MIDIMessage::realTimeMessage(_status);
    }

    bool isClockMessage() const {
        return MIDIMessage::isClockMessage(_status);
    }

    bool isTick() const { return isRealTimeMessage<Tick>(_status); }
    bool isStart() const { return isRealTimeMessage<Start>(_status); }
    bool isContinue() const { return isRealTimeMessage<Continue>(_status); }
    bool isStop() const { return isRealTimeMessage<Stop>(_status); }
    bool isActiveSensing() const { return isRealTimeMessage<ActiveSensing>(_status); }
    bool isReset() const { return isRealTimeMessage<Reset>(_status); }

    // Constructor

    MIDIMessage(uint8_t status = 0, uint8_t data0 = 0, uint8_t data1 = 0) :
        _status(status)
    {
        _data[0] = data0;
        _data[1] = data1;
    }

    // Factory

    static MIDIMessage makeNoteOff(uint8_t channel, uint8_t note, uint8_t velocity = 0) {
        return MIDIMessage(NoteOff | channel, note, velocity);
    }

    static MIDIMessage makeNoteOn(uint8_t channel, uint8_t note, uint8_t velocity = 127) {
        return MIDIMessage(NoteOn | channel, note, velocity);
    }

    static MIDIMessage makeKeyPressure(uint8_t channel, uint8_t note, uint8_t pressure) {
        return MIDIMessage(KeyPressure | channel, note, pressure);
    }

    static MIDIMessage makeControlChange(uint8_t channel, uint8_t controllerNumber, uint8_t controllerValue) {
        return MIDIMessage(ControlChange | channel, controllerNumber, controllerValue);
    }

    static MIDIMessage makeProgramChange(uint8_t channel, uint8_t programNumber) {
        return MIDIMessage(ProgramChange | channel, programNumber);
    }

    static MIDIMessage makeChannelPressure(uint8_t channel, uint8_t pressure) {
        return MIDIMessage(ChannelPressure | channel, pressure);
    }

    static MIDIMessage makePitchBend(uint8_t channel, int pitchBend) {
        pitchBend = std::min(0, std::max(0x3fff, pitchBend + 0x2000));
        return MIDIMessage(PitchBend | channel, pitchBend & 0x7f, (pitchBend >> 7) & 0x7f);
    }

    static void dump(const MIDIMessage &msg);

private:
    uint8_t _status;
    uint8_t _data[2];
};
