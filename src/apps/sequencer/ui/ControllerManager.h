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

    void update();

    bool recvMidi(MidiPort port, const MidiMessage &message);

private:
    void sendMidi(const MidiMessage &message);

    Model &_model;
    Engine &_engine;
    MidiPort _port;
    Container<LaunchpadController> _container;
    Controller *_controller = nullptr;

    friend class Controller;
};
