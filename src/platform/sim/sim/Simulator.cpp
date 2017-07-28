#include "Simulator.h"

#include "widgets/Button.h"
#include "widgets/Led.h"

namespace sim {

Simulator::Simulator() :
    _window("Sequencer", Vector2i(800, 400)),
    _drumKit(_audio)
{
    _timerFrequency = SDL_GetPerformanceFrequency();
    _timerStart = SDL_GetPerformanceCounter();

    std::fill(_gate.begin(), _gate.end(), false);
}

bool Simulator::terminate() const {
    return _window.terminate();
}

void Simulator::update() {
    for (auto &callback : _updateCallbacks) {
        callback();
    }

    _window.update();
}

void Simulator::render() {
    _window.render();
}

double Simulator::ticks() {
    double delta = SDL_GetPerformanceCounter() - _timerStart;
    return delta / _timerFrequency * 1000.0;
}

void Simulator::writeGate(int channel, bool value) {
    if (channel < 0 || channel >= _gate.size()) {
        return;
    }
    if (value != _gate[channel]) {
        _gate[channel] = value;
        if (value) {
            _drumKit.trigger(channel);
        }
    }
}

void Simulator::writeDAC(int channel, uint16_t value) {

}

uint16_t Simulator::readADC(int channel) {
    return 0;
}

void Simulator::addUpdateCallback(UpdateCallback callback) {
    _updateCallbacks.emplace_back(callback);
}

} // namespace sim
