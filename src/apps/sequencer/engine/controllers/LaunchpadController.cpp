#include "LaunchpadController.h"

LaunchpadController::LaunchpadController(ControllerManager &manager) :
    Controller(manager)
{}

void LaunchpadController::processMessage(const MidiMessage &message) {
    static int counter = 0;
    sendMessage(MidiMessage::makeNoteOn(0, message.note(), counter++));
}
