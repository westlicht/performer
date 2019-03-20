#include "Midi.h"

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
    auto &port = *static_cast<Midi::Port *>(userData);
    port.receive(*message);
}

static void errorCallback(RtMidiError::Type type, const std::string &errorText, void *userData) {
    auto &port = *static_cast<Midi::Port *>(userData);
    port.notifyError();
}

bool Midi::Port::send(uint8_t data) {
    if (_output.isPortOpen()) {
        std::vector<uint8_t> message = { data };
        _output.sendMessage(&message);
        return true;
    }

    return false;
}

bool Midi::Port::send(const uint8_t *data, size_t length) {
    if (_output.isPortOpen()) {
        std::vector<uint8_t> message(data, data + length);
        _output.sendMessage(&message);
        return true;
    }

    return false;
}

void Midi::Port::update() {
    if (!_open) {
        open();
    } else {
        if (_error || findPort(_input, _portIn) == -1 || findPort(_output, _portOut) == -1) {
            close();
        }

        std::lock_guard<std::mutex> lock(_recvQueueMutex);
        while (!_recvQueue.empty()) {
            _recvHandler(_recvQueue.front());
            _recvQueue.pop_front();
        }
    }
}

void Midi::Port::notifyError() {
    _error = true;
}

void Midi::Port::receive(const std::vector<uint8_t> &message) {
    std::lock_guard<std::mutex> lock(_recvQueueMutex);
    _recvQueue.emplace_back(message);
}

void Midi::Port::open() {
    if (_open) {
        return;
    }

    // open input
    try {
        int index = findPort(_input, _portIn);
        if (index >= 0) {
            _input.openPort(index);
            _input.ignoreTypes(false, false, false);
            _input.setCallback(recvCallback, this);
            _input.setErrorCallback(errorCallback, this);
        } else {
            _input.closePort();
            return;
        }
    } catch (RtMidiError &error) {
        if (_firstOpenAttempt) {
            std::cout << "Failed to open MIDI input port '" << _portIn << "'" << std::endl;
            error.printMessage();
            _firstOpenAttempt = false;
        }
        _input.closePort();
        return;
    }

    // open output
    try {
        int index = findPort(_output, _portOut);
        if (index >= 0) {
            _output.openPort(index);
            _output.setErrorCallback(errorCallback, this);
        } else {
            _output.closePort();
            return;
        }
    } catch (RtMidiError &error) {
        if (_firstOpenAttempt) {
            std::cout << "Failed to open MIDI output port '" << _portOut << "'" << std::endl;
            error.printMessage();
            _firstOpenAttempt = false;
        }
        _output.closePort();
        return;
    }

    if (_connectHandler) {
        _connectHandler();
    }

    _error = false;
    _open = true;
}

void Midi::Port::close() {
    if (!_open) {
        return;
    }

    _input.closePort();
    _output.closePort();

    if (_disconnectHandler) {
        _disconnectHandler();
    }

    _open = false;
    _firstOpenAttempt = true;
}

Midi::Midi() {
}

void Midi::registerPort(std::shared_ptr<Port> port) {
    _ports.emplace_back(port);
}

void Midi::dumpPorts() {
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

void Midi::update() {
    for (const auto &port : _ports) {
        port->update();
    }
}


} // namespace sim
