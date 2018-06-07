#pragma once

#include "RtMidi.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <functional>

#include <cstdint>

namespace sim {

class Midi {
public:
    class Port {
    public:
        typedef std::function<void(uint8_t data)> RecvHandler;
        typedef std::function<void()> ConnectHandler;
        typedef std::function<void()> DisconnectHandler;

        Port(
            const std::string &port,
            RecvHandler recvHandler
        ) : _port(port), _recvHandler(recvHandler) {}

        Port(
            const std::string &port,
            RecvHandler recvHandler,
            ConnectHandler connectHandler,
            DisconnectHandler disconnectHandler
        ) : _port(port), _recvHandler(recvHandler), _connectHandler(connectHandler), _disconnectHandler(disconnectHandler) {}

        bool isOpen() const { return _open; }

        bool send(uint8_t data);
        bool send(const uint8_t *data, size_t length);

        void update();
        void notifyError();

    private:
        void open();
        void close();

        std::string _port;
        RecvHandler _recvHandler;
        ConnectHandler _connectHandler;
        DisconnectHandler _disconnectHandler;

        bool _open = false;
        std::unique_ptr<RtMidiIn> _input;
        std::unique_ptr<RtMidiOut> _output;
    };

    Midi();

    void registerPort(std::shared_ptr<Port> port);

    void update();

private:
    void dumpPorts();

    std::vector<std::shared_ptr<Port>> _ports;
};

} // namespace sim
