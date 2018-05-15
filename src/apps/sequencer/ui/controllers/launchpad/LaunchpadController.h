#pragma once

#include "LaunchpadDevice.h"

#include "ui/Controller.h"

class LaunchpadController : public Controller {
public:
    LaunchpadController(ControllerManager &manager, Model &model, Engine &engine);

    virtual void update() override;

    virtual void recvMidi(const MidiMessage &message) override;

private:
    enum class Page : uint8_t {
        Pattern,
        Performer,
    };

    void buttonDown(int row, int col);
    void buttonUp(int row, int col);

    LaunchpadDevice _device;
};
