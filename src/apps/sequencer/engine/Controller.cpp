#include "Controller.h"

#include "core/Debug.h"

#include "controllers/LaunchpadController.h"

Controller::Controller(ControllerManager &manager) :
    _manager(manager)
{}

void Controller::sendMessage(const MidiMessage &message) {
    _manager.sendMessage(message);
}

ControllerManager::ControllerManager(UsbMidi &midi) :
    _midi(midi)
{
    _controller = new LaunchpadController(*this);// &_testController;
}

void ControllerManager::update() {
    MidiMessage message;
    while (_midi.recv(&message)) {
        DBG("recv:");
        MidiMessage::dump(message);
        _controller->processMessage(message);
    }
}

void ControllerManager::sendMessage(const MidiMessage &message) {
    DBG("send:");
    MidiMessage::dump(message);
    _midi.send(message);
}
