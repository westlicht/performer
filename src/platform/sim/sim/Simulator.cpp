#include "Simulator.h"

#include "widgets/Button.h"
#include "widgets/Led.h"

#include "instruments/DrumSampler.h"
#include "instruments/Synth.h"

#include <memory>

namespace sim {

const std::vector<std::string> Simulator::_midiPortName = {
    "Fireface UCX (23456129) Port 1", // MIDIHardwarePort
    "Port2", // MIDIUSBHostPort
};

Simulator::Simulator() :
    _window("Sequencer", Vector2i(800, 400))
{
    _timerFrequency = SDL_GetPerformanceFrequency();
    _timerStart = SDL_GetPerformanceCounter();

    setupInstruments();

    std::fill(_gate.begin(), _gate.end(), false);
    std::fill(_gate.begin(), _gate.end(), 0);
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
    if (channel >= 0 && channel < _gate.size()) {
        _gate[channel] = value;
    }
    if (channel >= 0 && channel < _instruments.size()) {
        _instruments[channel]->setGate(value);
    }
}

void Simulator::writeDAC(int channel, uint16_t value) {
    if (channel >= 0 && channel < _dac.size()) {
        _dac[channel] = value;
    }
    if (channel >= 0 && channel < _instruments.size()) {
        _instruments[channel]->setCV(value / float(8192));
    }
}

uint16_t Simulator::readADC(int channel) {
    return 0;
}

void Simulator::sendMIDI(int port, uint8_t data) {
    _midi.send(_midiPortName[port], data);
}

void Simulator::sendMIDI(int port, const uint8_t *data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        _midi.send(_midiPortName[port], data[i]);
    }
}

void Simulator::recvMIDI(int port, MIDIRecvCallback callback) {
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
    std::string prefix("assets/drumkit/");
    for (const auto &wav : { "kick.wav", "snare.wav", "rim.wav", "clap.wav", "hh1.wav", "hh2.wav", "tom1.wav" }) {
        _instruments.emplace_back(std::make_shared<DrumSampler>(_audio, prefix + wav));
    }
    _instruments.emplace_back(std::make_shared<Synth>(_audio));
}


} // namespace sim
