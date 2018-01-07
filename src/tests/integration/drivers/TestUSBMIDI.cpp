#include "IntegrationTest.h"

#include "drivers/USBH.h"
#include "drivers/USBMIDI.h"

class TestUSBMIDI : public IntegrationTest {
public:
    TestUSBMIDI() :
        IntegrationTest("USBMIDI", true),
        usbh(usbMidi)
    {}

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

INTEGRATION_TEST(TestUSBMIDI)
