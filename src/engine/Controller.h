#pragma once

#include "core/midi/MIDIMessage.h"
#include "core/midi/MIDIParser.h"

#include "drivers/USBMIDI.h"

class ControllerManager;

class Controller {
public:
    Controller(ControllerManager &manager);

    virtual void processMessage(const MIDIMessage &message) = 0;
    void sendMessage(const MIDIMessage &message);

protected:
    ControllerManager &_manager;
};

class ControllerManager {
public:
    ControllerManager(USBMIDI &midi);

    void update();
    void sendMessage(const MIDIMessage &message);

private:
    USBMIDI &_midi;
    MIDIParser _midiParser;
    Controller *_controller = nullptr;
};
