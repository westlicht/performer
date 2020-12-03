#include "Controller.h"

#include "ui/ControllerManager.h"

Controller::Controller(ControllerManager &manager, Model &model, Engine &engine) :
    _manager(manager),
    _model(model),
    _engine(engine)
{}

Controller::~Controller() {
}

bool Controller::sendMidi(uint8_t cable, const MidiMessage &message) {
    return _manager.sendMidi(cable, message);
}
