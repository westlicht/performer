#include "IntegrationTest.h"

#include "drivers/MIDI.h"

class TestMIDI : public IntegrationTest {
public:
    TestMIDI() {
    }

    void init() {
        midi.init();
    }

    void update() {
        MIDIMessage message;
        while (midi.recv(&message)) {
            MIDIMessage::dump(message);
            midi.send(message);
        }
    }

private:
    MIDI midi;
};

INTEGRATION_TEST("MIDI", TestMIDI)
