#pragma once

#include "RtMidi.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include <deque>
#include <mutex>

#include <cstdint>

namespace sim {

class Midi {
public:
    class Port {
    public:
        typedef std::function<void(const std::vector<uint8_t> &message)> RecvHandler;
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
        void receive(const std::vector<uint8_t> &message);

    private:
        void open();
        void close();

        std::string _port;
        RecvHandler _recvHandler;
        ConnectHandler _connectHandler;
        DisconnectHandler _disconnectHandler;

        bool _open = false;
        RtMidiIn _input;
        RtMidiOut _output;

        bool _firstOpenAttempt = true;
        bool _error = false;

        std::deque<std::vector<uint8_t>> _recvQueue;
        std::mutex _recvQueueMutex;
    };

    Midi();

    void registerPort(std::shared_ptr<Port> port);

    void update();

private:
    void dumpPorts();

    std::vector<std::shared_ptr<Port>> _ports;
};

} // namespace sim
