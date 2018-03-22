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
        switch (mode) {
        case Receive: {
            MidiMessage msg;
            if (midi.recv(&msg)) {
                MidiMessage::dump(msg);
            }
            break;
        }
        case Arpeggio: {
            DBG("step: %d", step);
            midi.send(MidiMessage::makeNoteOff(0, 36 + step % 24));
            ++step;
            midi.send(MidiMessage::makeNoteOn(0, 36 + step % 24));
            os::delay(500);
            break;
        }
        }
    }

private:
    enum Mode {
        Receive,
        Arpeggio,
    };

    MIDI midi;
    Mode mode = Receive;
    int step = 0;
};

INTEGRATION_TEST(TestMIDI)
