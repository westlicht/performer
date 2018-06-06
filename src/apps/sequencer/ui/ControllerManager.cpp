#include "ControllerManager.h"

ControllerManager::ControllerManager(Model &model, Engine &engine) :
    _model(model),
    _engine(engine)
{
    _port = MidiPort::UsbMidi;
    // TODO get this from settings
    _controller = _container.create<LaunchpadController>(*this, _model, _engine);
}

void ControllerManager::update() {
    if (_controller) {
        _controller->update();
    }
}

bool ControllerManager::recvMidi(MidiPort port, const MidiMessage &message) {
    if (_controller && port == _port) {
        _controller->recvMidi(message);
        return true;
    }

    return false;
}

bool ControllerManager::sendMidi(const MidiMessage &message) {
    return _engine.sendMidi(_port, message);
}
