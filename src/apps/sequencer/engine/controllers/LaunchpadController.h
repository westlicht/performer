#pragma once

#include "engine/Controller.h"

class LaunchpadController : public Controller {
public:
    LaunchpadController(ControllerManager &manager);

    virtual void processMessage(const MIDIMessage &message) override;
};
