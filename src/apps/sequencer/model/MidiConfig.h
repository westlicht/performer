#pragma once

#include "Types.h"
#include "Bitfield.h"
#include "ModelUtils.h"
#include "Serialize.h"

#include "core/utils/StringBuilder.h"
#include "core/math/Math.h"

template<bool IsSource>
class MidiConfig {
public:
    void edit(int value, bool shift) {
        if (shift) {
            editPort(value, false);
        } else {
            setIndexed(indexed() + value);
        }
    }

    void print(StringBuilder &str) const {
        printPort(str);
        str(" (");
        printChannel(str);
        str(")");
    }

    // port

    Types::MidiPort port() const { return _port; }

    void setPort(Types::MidiPort port) {
        _port = ModelUtils::clampedEnum(port);
    }

    void editPort(int value, bool shift) {
        setPort(ModelUtils::adjustedEnum(port(), value));
    }

    void printPort(StringBuilder &str) const {
        str(Types::midiPortName(port()));
    }

    // channel

    int channel() const {
        return _channel;
    }

    void setChannel(int channel) {
        _channel = clamp(channel, IsSource ? -1 : 0, 15);
    }

    void editChannel(int value, bool shift) {
        setChannel(channel() + value);
    }

    void printChannel(StringBuilder &str) const {
        Types::printMidiChannel(str, channel());
    }

    bool isOmni() const {
        return _channel == -1;
    }


    MidiConfig() {
        clear();
    }

    void clear() {
        setPort(Types::MidiPort::Midi);
        setChannel(IsSource ? -1 : 0);
    }

    void write(WriteContext &context) const {
        auto &writer = context.writer;
        writer.write(_port);
        writer.write(_channel);
    }

    void read(ReadContext &context) {
        auto &reader = context.reader;
        reader.read(_port);
        reader.read(_channel);
    }

    bool operator==(const MidiConfig<IsSource> &other) const {
        return _port == other._port && _channel == other._channel;
    }

    bool operator!=(const MidiConfig<IsSource> &other) const {
        return !(*this == other);
    }

private:
    int indexed() const {
        return int(port()) * (IsSource ? 17 : 16) + channel() + (IsSource ? 1 : 0);
    }

    void setIndexed(int index) {
        index = clamp(index, 0, int(Types::MidiPort::Last) * (IsSource ? 17 : 16) - 1);
        setPort(Types::MidiPort(index / (IsSource ? 17 : 16)));
        setChannel(index % (IsSource ? 17 : 16) - (IsSource ? 1 : 0));
    }

    Types::MidiPort _port;
    int8_t _channel;
};

typedef MidiConfig<true> MidiSourceConfig;
typedef MidiConfig<false> MidiTargetConfig;
