#include "Controller.h"

#include "ui/ControllerManager.h"

Controller::Controller(ControllerManager &manager, Model &model, Engine &engine) :
    _manager(manager),
    _model(model),
    _engine(engine)
{}

bool Controller::sendMidi(const MidiMessage &message) {
    return _manager.sendMidi(message);
}
