#include "IntegrationTest.h"

#include "drivers/Dio.h"

class TestDio : public IntegrationTest {
public:
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

private:
    Dio dio;
    bool clockOutput = false;
    bool resetOutput = true;
    int clockInputCount = 0;
    int resetInputCount = 0;
};

INTEGRATION_TEST(TestDio, "Dio", true)
