#include "IntegrationTest.h"

#include "drivers/Dac.h"

class TestDac : public IntegrationTest {
public:
    void init() override {
        dac.init();
    }

    void update() override {
        DBG("value = %d", value);
        for (int channel = 0; channel < 8; ++channel) {
            dac.setValue(channel, (value + channel * 0x1000) % 0x8000);
        }
        dac.write();
        value = (value + 0x1000) % 0x8000;
        os::delay(os::time::ms(50));
    }

private:
    Dac dac;
    uint16_t value = 0;
};

INTEGRATION_TEST(TestDac, "Dac", true)
