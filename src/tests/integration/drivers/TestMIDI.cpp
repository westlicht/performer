#include "IntegrationTest.h"

#include "drivers/MIDI.h"

class TestMIDI : public IntegrationTest {
public:
    TestMIDI() :
        IntegrationTest("MIDI", true)
    {}

    void init() override {
        midi.init();
    }

    void update() override {
        MIDIMessage message;
        while (midi.recv(&message)) {
            MIDIMessage::dump(message);
            midi.send(message);
        }
    }

private:
    MIDI midi;
};

INTEGRATION_TEST(TestMIDI)
