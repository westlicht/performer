#include "IntegrationTest.h"

#include "drivers/Midi.h"

class TestMidi : public IntegrationTest {
public:
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

    Midi midi;
    Mode mode = Receive;
    int step = 0;
};

INTEGRATION_TEST(TestMidi, "Midi", true)
