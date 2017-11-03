#include "LaunchpadController.h"

LaunchpadController::LaunchpadController(ControllerManager &manager) :
    Controller(manager)
{}

void LaunchpadController::processMessage(const MIDIMessage &message) {
    static int counter = 0;
    sendMessage(MIDIMessage::makeNoteOn(0, message.note(), counter++));
}
