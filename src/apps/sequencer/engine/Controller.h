#pragma once

#include "core/midi/MidiMessage.h"
#include "core/midi/MidiParser.h"

#include "drivers/USBMIDI.h"

class ControllerManager;

class Controller {
public:
    Controller(ControllerManager &manager);

    virtual void processMessage(const MidiMessage &message) = 0;
    void sendMessage(const MidiMessage &message);

protected:
    ControllerManager &_manager;
};

class ControllerManager {
public:
    ControllerManager(USBMIDI &midi);

    void update();
    void sendMessage(const MidiMessage &message);

private:
    USBMIDI &_midi;
    MidiParser _midiParser;
    Controller *_controller = nullptr;
};
