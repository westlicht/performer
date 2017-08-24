#include "test/TestRunner.h"

#include "drivers/ADC.h"

class TestADC : public Test {
public:
    TestADC() {
    }

    void init() {
        adc.init();
    }

    void update() {
        for (int c = 0; c < ADC::Channels; ++c) {
            DBG("adc%d: %d", c, adc.channel(c));
        }
    }

private:
    ADC adc;
};

TEST(TestADC)
