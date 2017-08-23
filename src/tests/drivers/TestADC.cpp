#include "test/TestRunner.h"

#include "drivers/ADC.h"

void test() {
    ADC adc;

    TestRunner::loop([&] () {
        DBG("");
        for (int c = 0; c < ADC::Channels; ++c) {
            DBG("adc%d: %d", c, adc.channel(c));
        }
        TestRunner::sleep(100);
    });
}
