#include "ControllerManager.h"

static const ControllerInfo controllerInfos[] = {
    { 0x1235, 0x0020, ControllerInfo::Type::Launchpad },    // Novation Launchpad S
    { 0x1235, 0x0036, ControllerInfo::Type::Launchpad },    // Novation Launchpad Mini MK1
    { 0x1235, 0x0037, ControllerInfo::Type::Launchpad },    // Novation Launchpad Mini MK2
};

static const ControllerInfo *findController(uint16_t vendorId, uint16_t productId) {
    for (size_t i = 0; i < sizeof(controllerInfos) / sizeof(controllerInfos[0]); ++i) {
        auto info = &controllerInfos[i];
        if (info->vendorId == vendorId && info->productId == productId) {
            return info;
        }
    }
    return nullptr;
}


ControllerManager::ControllerManager(Model &model, Engine &engine) :
    _model(model),
    _engine(engine)
{
    _port = MidiPort::UsbMidi;
}

void ControllerManager::connect(uint16_t vendorId, uint16_t productId) {
    auto info = findController(vendorId, productId);
    if (info) {
        switch (info->type) {
        case ControllerInfo::Type::Launchpad:
            _controller = _container.create<LaunchpadController>(*this, _model, _engine, *info);
            break;
        }
    }
}

void ControllerManager::disconnect() {
    if (_controller) {
        _container.destroy(_controller);
        _controller = nullptr;
    }
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
