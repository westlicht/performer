#include "TargetTracePlayer.h"

#include "Simulator.h"

#include <functional>

namespace sim {

struct TracePlayerBase {
    virtual ~TracePlayerBase() = 0;
    virtual void play(uint32_t tick) = 0;
};

template<typename T>
struct TracePlayer : public TracePlayerBase {
    using Record = typename T::Record;

    TracePlayer(const T &trace, std::function<void(const Record &)> func) :
        trace(trace),
        func(func)
    {}

    ~TracePlayer() {}

    void play(uint32_t tick) override {
        while (pos < trace.items().size() && trace.items()[pos].first == tick) {
            func(trace.items()[pos].second);
            ++pos;
        }
    }

    size_t pos = 0;
    const T &trace;
    std::function<void(const Record &)> func;
};

TargetTracePlayer::TargetTracePlayer(const TargetTrace &targetTrace, TargetInputHandler *targetInputHandler, TargetOutputHandler *targetOutputHandler) :
    _targetTrace(targetTrace),
    _targetInputHandler(targetInputHandler),
    _targetOutputHandler(targetOutputHandler)
{
    if (_targetInputHandler) {
        _tracePlayers.emplace_back(new TracePlayer<ButtonTrace>(_targetTrace.button, [this] (const ButtonState &buttonState) {
            for (size_t i = 0; i < buttonState.state.size(); ++i) {
                _targetInputHandler->writeButton(i, buttonState.state[i]);
            }
        }));
        _tracePlayers.emplace_back(new TracePlayer<AdcTrace>(_targetTrace.adc, [this] (const AdcState &adcState) {
            for (size_t i = 0; i < adcState.state.size(); ++i) {
                _targetInputHandler->writeAdc(i, adcState.state[i]);
            }
        }));
        _tracePlayers.emplace_back(new TracePlayer<DigitalInputTrace>(_targetTrace.digitalInput, [this] (const DigitalInputState &digitalInputState) {
            for (size_t i = 0; i < digitalInputState.state.size(); ++i) {
                _targetInputHandler->writeDigitalInput(i, digitalInputState.state[i]);
            }
        }));
        _tracePlayers.emplace_back(new TracePlayer<EncoderTrace>(_targetTrace.encoder, [this] (const EncoderEvent &encoderEvent) {
            _targetInputHandler->writeEncoder(encoderEvent);
        }));
        _tracePlayers.emplace_back(new TracePlayer<MidiTrace>(_targetTrace.midiInput, [this] (const MidiEvent &midiEvent) {
            _targetInputHandler->writeMidiInput(midiEvent);
        }));
    }

    if (_targetOutputHandler) {
        _tracePlayers.emplace_back(new TracePlayer<LedTrace>(_targetTrace.led, [this] (const LedState &ledState) {
            for (size_t i = 0; i < ledState.state.size() / 2; ++i) {
                _targetOutputHandler->writeLed(i, ledState.state[i * 2], ledState.state[i * 2 + 1]);
            }
        }));
        _tracePlayers.emplace_back(new TracePlayer<GateOutputTrace>(_targetTrace.gateOutput, [this] (const GateOutputState &gateOutputState) {
            for (size_t i = 0; i < gateOutputState.state.size(); ++i) {
                _targetOutputHandler->writeGateOutput(i, gateOutputState.state[i]);
            }
        }));
        _tracePlayers.emplace_back(new TracePlayer<DacTrace>(_targetTrace.dac, [this] (const DacState &dacState) {
            for (size_t i = 0; i < dacState.state.size(); ++i) {
                _targetOutputHandler->writeDac(i, dacState.state[i]);
            }
        }));
        _tracePlayers.emplace_back(new TracePlayer<DigitalOutputTrace>(_targetTrace.digitalOutput, [this] (const DigitalOutputState &digitalOutputState) {
            for (size_t i = 0; i < digitalOutputState.state.size(); ++i) {
                _targetOutputHandler->writeDigitalOutput(i, digitalOutputState.state[i]);
            }
        }));
        _tracePlayers.emplace_back(new TracePlayer<LcdTrace>(_targetTrace.lcd, [this] (const LcdState &lcdState) {
            _targetOutputHandler->writeLcd(lcdState.state);
        }));
        _tracePlayers.emplace_back(new TracePlayer<MidiTrace>(_targetTrace.midiOutput, [this] (const MidiEvent &midiEvent) {
            _targetOutputHandler->writeMidiOutput(midiEvent);
        }));
    }
}

TargetTracePlayer::~TargetTracePlayer() {}

void TargetTracePlayer::setTick(uint32_t tick) {
    for (auto &tracePlayer : _tracePlayers) {
        tracePlayer->play(tick);
    }
}

} // namespace sim
