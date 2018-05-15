#pragma once

#include "model/Model.h"

#include "engine/Engine.h"

#include "core/midi/MidiMessage.h"

class ControllerManager;

class Controller {
public:
    Controller(ControllerManager &manager, Model &model, Engine &engine);

    virtual void update() = 0;

    virtual void recvMidi(const MidiMessage &message) = 0;

protected:
    void sendMidi(const MidiMessage &message);

    ControllerManager &_manager;
    Model &_model;
    Engine &_engine;
};
