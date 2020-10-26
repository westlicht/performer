#pragma once

#include <algorithm>

#include <cstdint>
#include <cstring>

class MidiMessage {
public:
    using PayloadID = uint8_t;
    static constexpr PayloadID InvalidPayload = 0;

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
    static bool isChannelMessage(uint8_t status) { return (status & 0xf0) == ChannelMessage; }

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

    inline const uint8_t *raw() const { return _raw; }
    inline uint8_t length() const { return _length & 0x3; }

    // Channel messages

    bool isChannelMessage() const {
        return isChannelMessage(status());
    }

    ChannelMessage channelMessage() const {
        return MidiMessage::channelMessage(status());
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

    uint8_t controlNumber() const {
        return data0();
    }

    uint8_t controlValue() const {
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
        return MidiMessage::isSystemMessage(status());
    }

    SystemMessage systemMessage() const {
        return MidiMessage::systemMessage(status());
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
        return MidiMessage::isRealTimeMessage(status());
    }

    RealTimeMessage realTimeMessage() const {
        return MidiMessage::realTimeMessage(status());
    }

    bool isClockMessage() const {
        return MidiMessage::isClockMessage(status());
    }

    bool isTick() const { return isRealTimeMessage<Tick>(status()); }
    bool isStart() const { return isRealTimeMessage<Start>(status()); }
    bool isContinue() const { return isRealTimeMessage<Continue>(status()); }
    bool isStop() const { return isRealTimeMessage<Stop>(status()); }
    bool isActiveSensing() const { return isRealTimeMessage<ActiveSensing>(status()); }
    bool isReset() const { return isRealTimeMessage<Reset>(status()); }

    // Payload

    bool hasPayload() const {
        return payloadID() != InvalidPayload;
    }

    void clearPayload() {
        auto id = payloadID();
        if (id != InvalidPayload) {
            decPayloadRefCount(id);
            setPayloadID(InvalidPayload);
        }
    }

    void setPayload(const uint8_t *data, size_t size) {
        clearPayload();
        auto id = allocatePayload(size);
        if (id != InvalidPayload) {
            setPayloadID(id);
            std::memcpy(payloadData(id), data, size);
        }
    }

    const uint8_t *payloadData() const {
        return payloadData(payloadID());
    }

    size_t payloadLength() const {
        return payloadLength(payloadID());
    }

    void setPayloadID(PayloadID id) {
        _length = (_length & 0x3) | (id << 2);
    }

    PayloadID payloadID() const {
        return _length >> 2;
    }

    // Utilities

    void fixFakeNoteOff() {
        if (isNoteOn() && velocity() == 0) {
            _raw[0] = NoteOff | (_raw[0] & 0xf);
        }
    }

    // Constructor

    MidiMessage() = default;
    MidiMessage(const MidiMessage &other) {
        _raw[0] = other._raw[0];
        _raw[1] = other._raw[1];
        _raw[2] = other._raw[2];
        _length = other._length;

        if (hasPayload()) {
            incPayloadRefCount(payloadID());
        }
    }
    MidiMessage(uint8_t status) :
        _raw { status }, _length(1)
    {}
    MidiMessage(uint8_t status, uint8_t data0) :
        _raw { status, data0 }, _length(2)
    {}
    MidiMessage(uint8_t status, uint8_t data0, uint8_t data1) :
        _raw { status, data0, data1 }, _length(3)
    {}
    MidiMessage(const uint8_t *raw, size_t length) {
        _raw[0] = length > 0 ? raw[0] : 0;
        _raw[1] = length > 1 ? raw[1] : 0;
        _raw[2] = length > 2 ? raw[2] : 0;
        _length = std::min(size_t(2), length);
    }

    ~MidiMessage() {
        if (hasPayload()) {
            decPayloadRefCount(payloadID());
        }
    }

    MidiMessage& operator=(const MidiMessage& other)
    {
        if (hasPayload()) {
            decPayloadRefCount(payloadID());
        }

        _raw[0] = other._raw[0];
        _raw[1] = other._raw[1];
        _raw[2] = other._raw[2];
        _length = other._length;

        if (hasPayload()) {
            incPayloadRefCount(payloadID());
        }

        return *this;
    }

    // Factory

    static MidiMessage makeNoteOff(uint8_t channel, uint8_t note, uint8_t velocity = 0) {
        return MidiMessage(NoteOff | channel, note, velocity);
    }

    static MidiMessage makeNoteOn(uint8_t channel, uint8_t note, uint8_t velocity = 127) {
        return MidiMessage(NoteOn | channel, note, velocity);
    }

    static MidiMessage makeKeyPressure(uint8_t channel, uint8_t note, uint8_t pressure) {
        return MidiMessage(KeyPressure | channel, note, pressure);
    }

    static MidiMessage makeControlChange(uint8_t channel, uint8_t controlNumber, uint8_t controlValue) {
        return MidiMessage(ControlChange | channel, controlNumber, controlValue);
    }

    static MidiMessage makeProgramChange(uint8_t channel, uint8_t programNumber) {
        return MidiMessage(ProgramChange | channel, programNumber);
    }

    static MidiMessage makeChannelPressure(uint8_t channel, uint8_t pressure) {
        return MidiMessage(ChannelPressure | channel, pressure);
    }

    static MidiMessage makePitchBend(uint8_t channel, int pitchBend) {
        pitchBend = std::min(0, std::max(0x3fff, pitchBend + 0x2000));
        return MidiMessage(PitchBend | channel, pitchBend & 0x7f, (pitchBend >> 7) & 0x7f);
    }

    static MidiMessage makeSystemExclusive(const uint8_t *data, size_t length) {
        MidiMessage message(SystemExclusive);
        message.setPayload(data, length);
        return message;
    }

    static void dump(const MidiMessage &msg);

    static void setPayloadPool(uint8_t *data, size_t length);

private:
    static PayloadID allocatePayload(size_t length);
    static void incPayloadRefCount(PayloadID id);
    static void decPayloadRefCount(PayloadID id);
    static uint8_t *payloadData(PayloadID id);
    static size_t payloadLength(PayloadID id);

    struct PayloadPool {
        uint8_t *data = nullptr;
        size_t length = 0;

        PayloadID payloadID = InvalidPayload;
        uint32_t payloadRefCount = 0;
        size_t payloadLength = 0;

        bool valid() const { return data != nullptr; }
    };

    static PayloadPool _payloadPool;

    uint8_t _raw[3];
    uint8_t _length = 0;
};
