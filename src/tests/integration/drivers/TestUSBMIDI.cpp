#include "IntegrationTest.h"

#ifdef PLATFORM_STM32
#include "drivers/USBH.h"
#endif

#include "drivers/USBMIDI.h"

class TestUSBMIDI : public IntegrationTest {
public:
    TestUSBMIDI() :
        IntegrationTest("USBMIDI", true)
#ifdef PLATFORM_STM32
        ,usbh(usbMidi)
#endif
    {}

    void init() override {
        usbMidi.init();
#ifdef PLATFORM_STM32
        usbh.init();
        usbh.powerOn();
#endif
    }

    void update() override {
#ifdef PLATFORM_STM32
        usbh.process();
#endif
        os::delay(1);
    }

private:
    USBMIDI usbMidi;
#ifdef PLATFORM_STM32
    USBH usbh;
#endif
};

INTEGRATION_TEST(TestUSBMIDI)
