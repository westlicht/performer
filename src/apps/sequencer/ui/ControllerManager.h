#pragma once

#include "Controller.h"
#include "controllers/launchpad/LaunchpadController.h"

#include "model/Model.h"

#include "engine/Engine.h"
#include "engine/MidiPort.h"

#include "core/midi/MidiMessage.h"
#include "core/utils/Container.h"

class ControllerManager {
public:
    ControllerManager(Model &model, Engine &engine);

    void connect(uint16_t vendorId, uint16_t productId);
    void disconnect();

    bool isConnected() { return _controller != nullptr; }

    void update();

    int fps() const { return 50; }

    bool recvMidi(MidiPort port, const MidiMessage &message);

private:
    bool sendMidi(const MidiMessage &message);

    Model &_model;
    Engine &_engine;
    MidiPort _port;
    Container<LaunchpadController> _controllerContainer;
    Controller *_controller = nullptr;

    friend class Controller;
};
