#include "IntegrationTest.h"

#include "drivers/DIO.h"

class TestDIO : public IntegrationTest {
public:
    TestDIO() :
        IntegrationTest("DIO", true)
    {}

    void init() override {
        dio.init();

        dio.clockInput.setHandler([this] (bool) {
            ++clockInputCount;
        });
        dio.resetInput.setHandler([this] (bool) {
            ++resetInputCount;
        });
    }

    void update() override {
        bool clockOutput = false;
        bool resetOutput = true;
        while (true) {
            DBG("clock input: %s (called: %d)", dio.clockInput.get() ? "high" : "low", clockInputCount);
            DBG("reset input: %s (called: %d)", dio.resetInput.get() ? "high" : "low", resetInputCount);
            DBG("clock output: %s", clockOutput ? "high" : "low");
            DBG("reset output: %s", clockOutput ? "high" : "low");
            dio.clockOutput.set(clockOutput);
            dio.resetOutput.set(resetOutput);
            clockOutput = !clockOutput;
            resetOutput = !resetOutput;
            os::delay(os::time::ms(500));
        }
    }

private:
    DIO dio;
    int clockInputCount;
    int resetInputCount;
};

INTEGRATION_TEST(TestDIO)
