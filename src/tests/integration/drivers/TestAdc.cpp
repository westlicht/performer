#include "IntegrationTest.h"

#include "drivers/Adc.h"

class TestAdc : public IntegrationTest {
public:
    TestAdc() :
        IntegrationTest("Adc", true)
    {}

    void init() override {
        adc.init();
    }

    void update() override {
        for (int c = 0; c < Adc::Channels; ++c) {
            DBG("adc%d: %d", c, adc.channel(c));
        }
    }

private:
    Adc adc;
};

INTEGRATION_TEST(TestAdc)
