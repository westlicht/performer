#include "Controller.h"

#include "core/Debug.h"

#include "controllers/LaunchpadController.h"

Controller::Controller(ControllerManager &manager) :
    _manager(manager)
{}

void Controller::sendMessage(const MIDIMessage &message) {
    _manager.sendMessage(message);
}

ControllerManager::ControllerManager(USBMIDI &midi) :
    _midi(midi)
{
    _controller = new LaunchpadController(*this);// &_testController;
}

void ControllerManager::update() {
    MIDIMessage message;
    while (_midi.recv(&message)) {
        DBG("recv:");
        MIDIMessage::dump(message);
        _controller->processMessage(message);
    }
}

void ControllerManager::sendMessage(const MIDIMessage &message) {
    DBG("send:");
    MIDIMessage::dump(message);
    _midi.send(message);
}
