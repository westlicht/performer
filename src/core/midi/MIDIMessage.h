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

    inline uint8_t status() const { return _raw[0]; }
    inline uint8_t data0() const { return _raw[1]; }
    inline uint8_t data1() const { return _raw[2]; }

    inline uint8_t *raw() const { return _raw; }
    inline uint8_t length() const { return _length; }

    // Channel messages

    bool isChannelMessage() const {
        return isChannelMessage(status());
    }

    ChannelMessage channelMessage() const {
        return MIDIMessage::channelMessage(status());
    }

    uint8_t channel() const {
        return status() & 0xf;
    }

    // NoteOff

    bool isNoteOff() const {
        return isChannelMessage<NoteOff>(status());
    }

    // NoteOn

    bool isNoteOn() const {
        return isChannelMessage<NoteOn>(status());
    }

    uint8_t note() const {
        return data0();
    }

    uint8_t velocity() const {
        return data1();
    }

    // KeyPressure

    bool isKeyPressure() const {
        return isChannelMessage<KeyPressure>(status());
    }

    uint8_t keyPressure() const {
        return data1();
    }

    // ControlChange

    bool isControlChange() const {
        return isChannelMessage<ControlChange>(status());
    }

    uint8_t controllerNumber() const {
        return data0();
    }

    uint8_t controllerValue() const {
        return data1();
    }

    // ProgramChange

    bool isProgramChange() const {
        return isChannelMessage<ProgramChange>(status());
    }

    uint8_t programNumber() const {
        return data0();
    }

    // ChannelPressure

    bool isChannelPressure() const {
        return isChannelMessage<ChannelPressure>(status());
    }

    uint8_t channelPressure() const {
        return data0();
    }

    // PitchBend

    bool isPitchBend() const {
        return isChannelMessage<PitchBend>(status());
    }

    int pitchBend() const {
        return (int(data1()) << 7 | int(data0())) - int(0x2000);
    }

    // System messages

    bool isSystemMessage() const {
        return MIDIMessage::isSystemMessage(status());
    }

    SystemMessage systemMessage() const {
        return MIDIMessage::systemMessage(status());
    }

    bool isSystemExclusive() const { return isSystemMessage<SystemExclusive>(status()); }
    bool isTimeCode() const { return isSystemMessage<TimeCode>(status()); }
    bool isSongPosition() const { return isSystemMessage<SongPosition>(status()); }
    bool isSongSelect() const { return isSystemMessage<SongSelect>(status()); }
    bool isTuneRequest() const { return isSystemMessage<TuneRequest>(status()); }

    int songPosition() const {
        return (int(data1()) << 7 | int(data0()));
    }

    uint8_t songNumber() const {
        return data0();
    }

    // Real-time messages

    bool isRealTimeMessage() const {
        return MIDIMessage::isRealTimeMessage(status());
    }

    RealTimeMessage realTimeMessage() const {
        return MIDIMessage::realTimeMessage(status());
    }

    bool isClockMessage() const {
        return MIDIMessage::isClockMessage(status());
    }

    bool isTick() const { return isRealTimeMessage<Tick>(status()); }
    bool isStart() const { return isRealTimeMessage<Start>(status()); }
    bool isContinue() const { return isRealTimeMessage<Continue>(status()); }
    bool isStop() const { return isRealTimeMessage<Stop>(status()); }
    bool isActiveSensing() const { return isRealTimeMessage<ActiveSensing>(status()); }
    bool isReset() const { return isRealTimeMessage<Reset>(status()); }

    // Constructor

    MIDIMessage() = default;
    MIDIMessage(uint8_t status) :
        _raw { status }, _length(1)
    {}
    MIDIMessage(uint8_t status, uint8_t data0) :
        _raw { status, data0 }, _length(2)
    {}
    MIDIMessage(uint8_t status, uint8_t data0, uint8_t data1) :
        _raw { status, data0, data1 }, _length(3)
    {}

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
    uint8_t _raw[3];
    uint8_t _length = 0;
    // uint8_t status() = 0;
    // uint8_t _data[2] = { 0, 0 };
    // uint8_t _dataLength = 0;
};
