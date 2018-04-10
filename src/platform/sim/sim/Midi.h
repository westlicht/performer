#pragma once

#include "RtMidi.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <functional>

namespace sim {

class Midi {
public:
    Midi();

    typedef std::function<void(uint8_t)> RecvCallback;

    void recv(const std::string &port, RecvCallback callback);
    void send(const std::string &port, uint8_t data);

private:
    void dumpPorts();

    std::unordered_map<std::string, std::unique_ptr<RtMidiIn>> _inputPorts;
    std::vector<std::unique_ptr<RecvCallback>> _inputCallbacks;
    std::unordered_map<std::string, std::unique_ptr<RtMidiOut>> _outputPorts;
};

} // namespace sim
