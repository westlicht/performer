#include "Simulator.h"

#include "libs/stb/stb_image_write.h"

#include "os/os.h"

#include "core/midi/MidiMessage.h"

#include <memory>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>

#include <cmath>

namespace sim {

static Simulator *g_instance;

Simulator::Simulator(Target target) :
    _target(target)
{
    g_instance = this;

    _writeTrace = true;
}

Simulator::~Simulator() {
    if (_targetCreated) {
       _target.destroy();
    }
}

void Simulator::wait(int ms) {
    while (ms--) {
        step();
    }
}

void Simulator::setButton(int index, bool pressed) {
    writeButton(index, pressed);
}

void Simulator::setEncoder(bool pressed) {
    writeEncoder(pressed ? EncoderEvent::Down : EncoderEvent::Up);
}

void Simulator::rotateEncoder(int direction) {
    if (direction > 0) {
        writeEncoder(EncoderEvent::Right);
    } else if (direction < 0) {
        writeEncoder(EncoderEvent::Left);
    }
}

void Simulator::setAdc(int channel, float voltage) {
    float normalized = std::max(0.f, std::min(1.f, voltage * 0.1f + 0.5f));
    writeAdc(channel, uint16_t(std::floor(0xffff - 0xffff * normalized)));
}

void Simulator::setDio(int pin, bool state) {
    writeDigitalInput(pin, state);
}

void Simulator::sendMidi(int port, const MidiMessage &message) {
    writeMidiInput(MidiEvent::makeMessage(port, message));
}

void Simulator::screenshot(const std::string &filename) {
    std::unique_ptr<uint8_t[]> pixelBuffer(new uint8_t[CONFIG_LCD_WIDTH * CONFIG_LCD_HEIGHT]);

    const uint8_t *src = _targetState.lcd.state.data();
    uint8_t *dst = pixelBuffer.get();

    for (int i = 0; i < CONFIG_LCD_WIDTH * CONFIG_LCD_HEIGHT; ++i) {
        float s = std::min(uint8_t(15), *src++) * (1.f / 15.f);
        *dst++ = s * 0xff;
    }

    stbi_write_png(filename.c_str(), CONFIG_LCD_WIDTH, CONFIG_LCD_HEIGHT, 1, pixelBuffer.get(), CONFIG_LCD_WIDTH);
}

double Simulator::ticks() {
    return _tick;
}

void Simulator::addUpdateCallback(UpdateCallback callback) {
    _updateCallbacks.emplace_back(callback);
}

void Simulator::registerTargetInputObserver(TargetInputHandler *observer) {
    _targetInputObservers.emplace_back(observer);
}

void Simulator::registerTargetOutputObserver(TargetOutputHandler *observer) {
    _targetOutputObservers.emplace_back(observer);
}

// TargetInputHandler

void Simulator::writeButton(int index, bool pressed) {
    _targetState.button.set(index, pressed);

    if (_writeTrace) {
        _targetTrace.button.write(_tick, _targetState.button);
    }

    for (auto observer : _targetInputObservers) {
        observer->writeButton(index, pressed);
    }
}

void Simulator::writeEncoder(EncoderEvent event) {
    if (_writeTrace) {
        _targetTrace.encoder.write(_tick, event);
    }

    for (auto observer : _targetInputObservers) {
        observer->writeEncoder(event);
    }
}

void Simulator::writeAdc(int channel, uint16_t value) {
    auto valueToVoltage = [] (uint16_t value) {
        float normalized = (0xffff - value) / float(0xffff);
        return (normalized - 0.5f) * 10.f;
    };

    _targetState.adc.set(channel, value, valueToVoltage(value));

    if (_writeTrace) {
        _targetTrace.adc.write(_tick, _targetState.adc);
    }

    for (auto observer : _targetInputObservers) {
        observer->writeAdc(channel, value);
    }
}

void Simulator::writeDigitalInput(int pin, bool value) {
    _targetState.digitalOutput.set(pin, value);

    if (_writeTrace) {
        _targetTrace.digitalOutput.write(_tick, _targetState.digitalOutput);
    }

    for (auto observer : _targetInputObservers) {
        observer->writeDigitalInput(pin, value);
    }
}

void Simulator::writeMidiInput(MidiEvent event) {
    if (_writeTrace) {
        _targetTrace.midiInput.write(_tick, event);
    }

    for (auto observer : _targetInputObservers) {
        observer->writeMidiInput(event);
    }
}

// TargetOutputHandler

void Simulator::writeLed(int index, bool red, bool green) {
    _targetState.led.set(index, red, green);

    if (_writeTrace) {
        _targetTrace.led.write(_tick, _targetState.led);
    }

    for (auto observer : _targetOutputObservers) {
        observer->writeLed(index, red, green);
    }
}

void Simulator::writeGateOutput(int channel, bool value) {
    _targetState.gateOutput.set(channel, value);

    if (_writeTrace) {
        _targetTrace.gateOutput.write(_tick, _targetState.gateOutput);
    }

    for (auto observer : _targetOutputObservers) {
        observer->writeGateOutput(channel, value);
    }
}

void Simulator::writeDac(int channel, uint16_t value) {
    auto valueToVoltage = [] (uint16_t value) {
        // In ideal DAC/OpAmp configuration we get:
        // 0     ->  5.17V
        // 32768 -> -5.25V
        // it follows:
        // 534   ->  5.00V
        // 31981 -> -5.00V
        static constexpr float value0 = 31981.f;
        static constexpr float value1 = 534.f;
        return (value - value0) / (value1 - value0) * 10.f - 5.f;
    };

    _targetState.dac.set(channel, value, valueToVoltage(value));

    if (_writeTrace) {
        _targetTrace.dac.write(_tick, _targetState.dac);
    }

    for (auto observer : _targetOutputObservers) {
        observer->writeDac(channel, value);
    }
}

void Simulator::writeDigitalOutput(int pin, bool value) {
    _targetState.digitalOutput.set(pin, value);

    if (_writeTrace) {
        _targetTrace.digitalOutput.write(_tick, _targetState.digitalOutput);
    }

    for (auto observer : _targetOutputObservers) {
        observer->writeDigitalOutput(pin, value);
    }
}

void Simulator::writeLcd(uint8_t *frameBuffer) {
    _targetState.lcd.set(frameBuffer);

    if (_writeTrace) {
        _targetTrace.lcd.write(_tick, _targetState.lcd);
    }

    for (auto observer : _targetOutputObservers) {
        observer->writeLcd(frameBuffer);
    }
}

void Simulator::writeMidiOutput(MidiEvent event) {
    if (_writeTrace) {
        _targetTrace.midiOutput.write(_tick, event);
    }

    for (auto observer : _targetOutputObservers) {
        observer->writeMidiOutput(event);
    }
}


Simulator &Simulator::instance() {
    return *g_instance;
}

void Simulator::step() {
    // std::cout << "step: tick=" << _tick << std::endl;

    if (!_targetCreated) {
        _target.create();
        _targetCreated = true;
    }

    for (const auto &callback : os::updateCallbacks()) {
        callback();
    }

    for (const auto &callback : _updateCallbacks) {
        callback();
    }

    _target.update();

    _tick += 1;
}

} // namespace sim
