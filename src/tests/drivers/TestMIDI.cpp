#include "test/TestRunner.h"

#include "drivers/MIDI.h"

class TestMIDI : public Test {
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

TEST(TestMIDI)
