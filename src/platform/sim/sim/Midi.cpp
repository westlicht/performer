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
    for (auto data : *message) {
        auto &handler = *static_cast<Midi::Port::RecvHandler *>(userData);
        handler(data);
    }
}

static void errorCallback(RtMidiError::Type type, const std::string &errorText, void *userData) {
    auto &port = *static_cast<Midi::Port *>(userData);
    port.notifyError();
}


bool Midi::Port::send(uint8_t data) {
    if (_output) {
        std::vector<uint8_t> message = { data };
        _output->sendMessage(&message);
        return true;
    }

    return false;
}

bool Midi::Port::send(const uint8_t *data, size_t length) {
    if (_output) {
        std::vector<uint8_t> message(data, data + length);
        _output->sendMessage(&message);
        return true;
    }

    return false;
}

void Midi::Port::update() {
    if (!_open) {
        open();
    } else {
        if (findPort(*_input, _port) == -1 || findPort(*_output, _port) == -1) {
            close();
        }
    }
}

void Midi::Port::notifyError() {
    close();
}

void Midi::Port::open() {
    if (_open) {
        return;
    }

    // open input
    try {
        _input.reset(new RtMidiIn());
        int index = findPort(*_input, _port);
        if (index >= 0) {
            _input->openPort(index);
            _input->ignoreTypes(false, false, false);
            _input->setCallback(recvCallback, &_recvHandler);
            _input->setErrorCallback(errorCallback, this);
        } else {
            _input.reset();
            return;
        }
    } catch (RtMidiError &error) {
        if (_firstOpenAttempt) {
            std::cout << "Failed to open MIDI input port '" << _port << "'" << std::endl;
            error.printMessage();
            _firstOpenAttempt = false;
        }
        _input.reset();
        return;
    }

    // open output
    try {
        _output.reset(new RtMidiOut());
        int index = findPort(*_output, _port);
        if (index >= 0) {
            _output->openPort(index);
            _output->setErrorCallback(errorCallback, this);
        } else {
            _output.reset();
            return;
        }
    } catch (RtMidiError &error) {
        if (_firstOpenAttempt) {
            std::cout << "Failed to open MIDI output port '" << _port << "'" << std::endl;
            error.printMessage();
            _firstOpenAttempt = false;
        }
        _output.reset();
        return;
    }

    if (_connectHandler) {
        _connectHandler();
    }

    _open = true;
}

void Midi::Port::close() {
    if (!_open) {
        return;
    }

    _input.reset();
    _output.reset();

    if (_disconnectHandler) {
        _disconnectHandler();
    }

    _open = false;
    _firstOpenAttempt = true;
}


Midi::Midi() {
    dumpPorts();
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
