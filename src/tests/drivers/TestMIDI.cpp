#include "test/TestRunner.h"

#include "drivers/MIDI.h"

void test() {
    MIDI midi;

    midi.init();

    TestRunner::loop([&] () {
        MIDIMessage message;
        while (midi.recv(&message)) {
            MIDIMessage::dump(message);
            midi.send(message);
        }
    });
}
