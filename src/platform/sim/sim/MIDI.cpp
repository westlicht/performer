#include "MIDI.h"

#include <algorithm>

namespace sim {

template<typename T>
static int findPort(T &midi, const std::string &port) {
    for (size_t i = 0; i < midi.getPortCount(); ++i) {
        if (midi.getPortName(i) == port) {
            return i;
        }
    }
    return -1;
}

static void recvCallback(double timeStamp, std::vector<uint8_t> *message, void *userData) {
    for (auto data : *message) {
        auto callback = *static_cast<MIDI::RecvCallback *>(userData);
        callback(data);
    }
}

MIDI::MIDI() {
    dumpPorts();
}

void MIDI::recv(const std::string &port, RecvCallback callback) {
    // open port and assign callback
    if (_inputPorts.find(port) == _inputPorts.end()) {
        std::unique_ptr<RtMidiIn> input(new RtMidiIn());
        try {
            std::cout << "Opening MIDI input port '" << port << "'" << std::endl;
            input.reset(new RtMidiIn());
            int index = findPort(*input, port);
            if (index >= 0) {
                input->openPort(index);
                input->ignoreTypes(false, false, false);
                _inputCallbacks.emplace_back(new RecvCallback(callback));
                input->setCallback(recvCallback, _inputCallbacks.back().get());
            } else {
                std::cout << "MIDI input port '" << port << "' not found!" << std::endl;
                input.reset();
            }
        } catch (RtMidiError &error) {
            std::cout << "Failed to open MIDI input port '" << port << "'" << std::endl;
            error.printMessage();
            input.reset();
        }
        _inputPorts[port] = std::move(input);
    } else {
        std::cout << "MIDI input port '" << port << "' is already opened!" << std::endl;
    }
}

void MIDI::send(const std::string &port, uint8_t data) {
    // open port on first send
    if (_outputPorts.find(port) == _outputPorts.end()) {
        std::unique_ptr<RtMidiOut> output;
        try {
            std::cout << "Opening MIDI output port '" << port << "'" << std::endl;
            output.reset(new RtMidiOut());
            int index = findPort(*output, port);
            if (index >= 0) {
                output->openPort(index);
            } else {
                std::cout << "MIDI output port '" << port << "' not found!" << std::endl;
                output.reset();
            }
        } catch (RtMidiError &error) {
            std::cout << "Failed to open MIDI output port '" << port << "'" << std::endl;
            error.printMessage();
            output.reset();
        }
        _outputPorts[port] = std::move(output);
    }

    const auto &output = _outputPorts.at(port);
    if (output) {
        std::vector<uint8_t> message = { data };
        output->sendMessage(&message);
    }
}

void MIDI::dumpPorts() {
    try {
        std::vector<std::string> ports;

        std::unique_ptr<RtMidiIn> input(new RtMidiIn());
        for (size_t i = 0; i < input->getPortCount(); ++i) {
            ports.emplace_back(input->getPortName(i));
        }

        std::unique_ptr<RtMidiOut> output(new RtMidiOut());
        for (size_t i = 0; i < output->getPortCount(); ++i) {
            if (!std::count(ports.begin(), ports.end(), output->getPortName(i))) {
                ports.emplace_back(output->getPortName(i));
            }
        }

        std::cout << "MIDI ports:" << std::endl;
        for (const auto &port : ports) {
            std::cout << "  - " << port << std::endl;
        }
    } catch (RtMidiError &error) {
        error.printMessage();
    }
}

} // namespace sim
