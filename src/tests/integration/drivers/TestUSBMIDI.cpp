#include "test/TestRunner.h"

#include "drivers/USBH.h"
#include "drivers/USBMIDI.h"

class TestUSBMIDI : public Test {
public:
    TestUSBMIDI() :
        usbh(usbMidi)
    {
    }

    void init() {
        usbMidi.init();
        usbh.init();
        usbh.powerOn();
    }

    void update() {
        usbh.process();
        os::delay(1);
    }

private:
    USBMIDI usbMidi;
    USBH usbh;
};

TEST(TestUSBMIDI)
