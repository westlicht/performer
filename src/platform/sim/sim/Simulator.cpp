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

    _midi.update();

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
    auto valueToVolts = [] (uint16_t value) {
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

    if (channel >= 0 && channel < int(_dac.size())) {
        _dac[channel] = value;
        _instruments->setCv(channel, valueToVolts(value));
    }
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
