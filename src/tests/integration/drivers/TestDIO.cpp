#include "IntegrationTest.h"

#include "drivers/DIO.h"

class TestDIO : public IntegrationTest {
public:
    TestDIO() {
    }

    void init() {
        dio.init();
    }

    void update() {
        bool clockOutput = false;
        bool resetOutput = true;
        while (true) {
            DBG("clock input: %s", dio.clockInput.get() ? "high" : "low");
            DBG("reset input: %s", dio.resetInput.get() ? "high" : "low");
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
};

INTEGRATION_TEST("DIO", TestDIO)
