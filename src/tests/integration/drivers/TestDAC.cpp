#include "test/TestRunner.h"

#include "drivers/DAC.h"

class TestDAC : public Test {
public:
    TestDAC() {
    }

    void init() {
        dac.init();
    }

    void update() {
        DBG("value = %d", value);
        for (int channel = 0; channel < 8; ++channel) {
            dac.setValue(channel, (channel * 8192 + value) & 0xffff);
        }
        dac.write();
        value += 1000;
    }

private:
    DAC dac;
    uint16_t value = 0;
};

TEST(TestDAC)
