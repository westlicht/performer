#include "TargetTrace.h"

#include "TargetUtils.h"

#include "tinyformat.h"

#include <iomanip>
#include <memory>

namespace sim {

template<typename T>
static void write(T data, std::ostream &stream) {
    stream.write(reinterpret_cast<const char *>(&data), sizeof(T));
}

template<typename T>
static void read(T &data, std::istream &stream) {
    stream.read(reinterpret_cast<char *>(&data), sizeof(T));
}

template<typename T>
static T read(std::istream &stream) {
    T data;
    stream.read(reinterpret_cast<char *>(&data), sizeof(T));
    return data;
}

static std::ostream &operator<<(std::ostream &os, const ButtonState &state) {
    for (int i = 0; i < ButtonState::Count; ++i) {
        os << (state.state[i] ? "x" : "-");
        if (i % 8 == 7 && i < ButtonState::Count - 1) {
            os << " ";
        }
    }
    return os;
}

static std::ostream &operator<<(std::ostream &os, const AdcState &state) {
    for (int i = 0; i < AdcState::Count; ++i) {
        os << tfm::format("%.3f", adcToVoltage(state.state[i]));
        if (i < AdcState::Count - 1) {
            os << " ";
        }
    }
    return os;
}

static std::ostream &operator<<(std::ostream &os, const DigitalInputState &state) {
    for (int i = 0; i < DigitalInputState::Count; ++i) {
        os << state.state[i];
    }
    return os;
}

static std::ostream &operator<<(std::ostream &os, const LedState &state) {
    for (int i = 0; i < LedState::Count; ++i) {
        bool red = state.state[i * 2];
        bool green = state.state[i * 2 + 1];
        os << ((red && green) ? "y" : (red ? "r" : (green ? "g" : "-")));
        if (i % 8 == 7 ) {
            os << " ";
        }
    }
    return os;
}

static std::ostream &operator<<(std::ostream &os, const GateOutputState &state) {
    for (int i = 0; i < GateOutputState::Count; ++i) {
        os << state.state[i];
    }
    return os;
}

static std::ostream &operator<<(std::ostream &os, const DacState &state) {
    for (int i = 0; i < DacState::Count; ++i) {
        os << tfm::format("%.3f", dacToVoltage(state.state[i]));
        if (i < DacState::Count - 1) {
            os << " ";
        }
    }
    return os;
}

static std::ostream &operator<<(std::ostream &os, const DigitalOutputState &state) {
    for (int i = 0; i < DigitalOutputState::Count; ++i) {
        os << state.state[i];
    }
    return os;
}

static std::ostream &operator<<(std::ostream &os, const LcdState &state) {
    return os;
}

static std::ostream &operator<<(std::ostream &os, const EncoderEvent &event) {
    switch (event) {
    case EncoderEvent::Down:    os << "down";   break;
    case EncoderEvent::Up:      os << "up";     break;
    case EncoderEvent::Left:    os << "left";   break;
    case EncoderEvent::Right:   os << "right";  break;
    }
    return os;
}

static std::ostream &operator<<(std::ostream &os, const MidiEvent &event) {
    switch (event.kind) {
    case MidiEvent::Connect:
        os << tfm::format("connect (vid: %04x pid: %04x", event.connect.vendorId, event.connect.productId);
        break;
    case MidiEvent::Disconnect:
        os << "disconnect";
        break;
    case MidiEvent::Message:
        os << "msg ";
        for (int i = 0; i < event.message.length(); ++i) {
            os << std::hex << event.message.raw()[i];
            if (i < event.message.length() - 1) {
                os << " ";
            };
        }
    }
    return os;
}

struct WriterBase {
    virtual ~WriterBase() {}
    virtual uint32_t write(uint32_t tick, std::ostream &os) = 0;
};

template<typename T>
struct Writer : public WriterBase {
    using Item = typename T::Item;
    using Iterator = typename std::vector<Item>::const_iterator;

    std::string id;
    Iterator it;
    Iterator end;

    Writer(const T &trace, const std::string &id) :
        id(id),
        it(trace.items().begin()),
        end(trace.items().end())
    {}

    uint32_t write(uint32_t tick, std::ostream &os) {
        while (it < end && it->first <= tick) {
            os << tfm::format("%06d %3s | ", it->first, id);
            os << it->second << std::endl;
            ++it;
        }
        return it == end ? 0xffffffff : it->first;
    }
};

void TargetTrace::writeStream(std::ostream &stream) const {
    button.writeStream(stream);
    adc.writeStream(stream);
    digitalInput.writeStream(stream);
    led.writeStream(stream);
    gateOutput.writeStream(stream);
    dac.writeStream(stream);
    digitalOutput.writeStream(stream);
    lcd.writeStream(stream);
    encoder.writeStream(stream);
    midiInput.writeStream(stream);
    midiOutput.writeStream(stream);
}

void TargetTrace::readStream(std::istream &stream) {
    button.readStream(stream);
    adc.readStream(stream);
    digitalInput.readStream(stream);
    led.readStream(stream);
    gateOutput.readStream(stream);
    dac.readStream(stream);
    digitalOutput.readStream(stream);
    lcd.readStream(stream);
    encoder.readStream(stream);
    midiInput.readStream(stream);
    midiOutput.readStream(stream);
}

void TargetTrace::saveToFile(const std::string &filename) const {
    std::ofstream ofs(filename, std::ios::binary);
    writeStream(ofs);
    ofs.close();
}

void TargetTrace::loadFromFile(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    readStream(ifs);
    ifs.close();
}

void TargetTrace::saveToText(const std::string &filename) const {
    std::vector<std::unique_ptr<WriterBase>> writers;
    writers.emplace_back(new Writer<ButtonTrace>(button, "BTN"));
    writers.emplace_back(new Writer<AdcTrace>(adc, "ADC"));
    writers.emplace_back(new Writer<DigitalInputTrace>(digitalInput, "DI"));
    writers.emplace_back(new Writer<LedTrace>(led, "LED"));
    writers.emplace_back(new Writer<GateOutputTrace>(gateOutput, "GAT"));
    writers.emplace_back(new Writer<DacTrace>(dac, "DAC"));
    writers.emplace_back(new Writer<DigitalOutputTrace>(digitalOutput, "DO"));
    writers.emplace_back(new Writer<LcdTrace>(lcd, "LCD"));
    writers.emplace_back(new Writer<EncoderTrace>(encoder, "ENC"));
    writers.emplace_back(new Writer<MidiTrace>(midiInput, "MI"));
    writers.emplace_back(new Writer<MidiTrace>(midiOutput, "MO"));

    std::ofstream ofs(filename);

    uint32_t tick = 0;
    while (tick < 0xffffffff) {
        uint32_t nextTick = 0xffffffff;
        for (const auto &writer : writers) {
            nextTick = std::min(nextTick, writer->write(tick, ofs));
        }
        tick = nextTick;
    }

    ofs.close();
}

} // namespace sim
