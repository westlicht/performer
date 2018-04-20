#include "Simulator.h"

#include "os/os.h"

#include "widgets/Button.h"
#include "widgets/Led.h"

#include "instruments/DrumSampler.h"
#include "instruments/Synth.h"

#include <memory>
#include <sstream>
#include <iomanip>

namespace sim {

const std::vector<std::string> Simulator::_midiPortName = {
    "SL MkII Port 1", // MidiHardwarePort
    "SL MkII Port 2", // MidiUsbHostPort
};

Simulator::Simulator() :
    _window("Sequencer", Vector2i(800, 500))
{
    _timerFrequency = SDL_GetPerformanceFrequency();
    _timerStart = SDL_GetPerformanceCounter();

    setupInstruments();

    std::fill(_gate.begin(), _gate.end(), false);
    std::fill(_gate.begin(), _gate.end(), 0);

    // button to take screenshots
    _screenshotButton = _window.createWidget<Button>(
        Vector2i(8, 8),
        Vector2i(8, 8),
        SDLK_F10
    );
    _screenshotButton->setCallback([&] (bool pressed) {
        if (pressed) {
            screenshot();
        }
    });
}

void Simulator::close() {
    _window.close();
}

bool Simulator::terminate() const {
    return _window.terminate();
}

void Simulator::update() {
    for (const auto &callback : os::updateCallbacks()) {
        callback();
    }
    for (const auto &callback : _updateCallbacks) {
        callback();
    }

    _window.update();
}

void Simulator::render() {
    double currentTicks = ticks();
    if (currentTicks < _lastRenderTicks + 15) {
        return;
    }
    _lastRenderTicks = currentTicks;
    _window.render();
}

void Simulator::delay(int ms) {
    SDL_Delay(ms);
}

double Simulator::ticks() {
    double delta = SDL_GetPerformanceCounter() - _timerStart;
    return delta / _timerFrequency * 1000.0;
}

void Simulator::setScreenshotCallback(ScreenshotCallback callback) {
    _screenshotCallback = callback;
}

void Simulator::screenshot(const std::string &filename) {
    if (_screenshotCallback) {
        std::stringstream ss;
        if (filename.empty()) {
            static int counter = 0;
            ss << "screenshot-" << std::setfill('0') << std::setw(3) << counter << ".png";
            ++counter;
        } else {
            ss << filename;
        }
        _screenshotCallback(ss.str());
    }
}

void Simulator::writeGate(int channel, bool value) {
    if (channel >= 0 && channel < int(_gate.size())) {
        _gate[channel] = value;
        _instruments->setGate(channel, value);
    }
}

void Simulator::writeDac(int channel, uint16_t value) {
    if (channel >= 0 && channel < int(_dac.size())) {
        _dac[channel] = value;
        _instruments->setCv(channel, 10.f - value / 3276.75f);
    }
}

void Simulator::sendMidi(int port, uint8_t data) {
    _midi.send(_midiPortName[port], data);
}

void Simulator::sendMidi(int port, const uint8_t *data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        _midi.send(_midiPortName[port], data[i]);
    }
}

void Simulator::recvMidi(int port, MidiRecvCallback callback) {
    _midi.recv(_midiPortName[port], [callback] (uint8_t data) { callback(data); });
}

Simulator &Simulator::instance() {
    static std::unique_ptr<Simulator> simulator;
    if (!simulator) {
        simulator.reset(new Simulator());
    }
    return *simulator;
}

void Simulator::addUpdateCallback(UpdateCallback callback) {
    _updateCallbacks.emplace_back(callback);
}

void Simulator::setupInstruments() {
    // _instruments.reset(new SamplerSetup(_audio));
    _instruments.reset(new MixedSetup(_audio));
}


} // namespace sim
