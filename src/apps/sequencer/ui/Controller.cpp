#include "Controller.h"

#include "ui/ControllerManager.h"

Controller::Controller(ControllerManager &manager, Model &model, Engine &engine) :
    _manager(manager),
    _model(model),
    _engine(engine)
{}

void Controller::sendMidi(const MidiMessage &message) {
    _manager.sendMidi(message);
}
