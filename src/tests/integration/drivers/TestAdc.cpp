#include "IntegrationTest.h"

#include "drivers/Adc.h"

class TestAdc : public IntegrationTest {
public:
    void init() override {
        adc.init();
    }

    void update() override {
        for (int c = 0; c < Adc::Channels; ++c) {
            dbg_printf("adc%d: %d ", c, adc.channel(c));
        }
        dbg_printf("\n");
    }

private:
    Adc adc;
};

INTEGRATION_TEST(TestAdc, "Adc", true)
