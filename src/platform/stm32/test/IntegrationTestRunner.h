
#include "core/Debug.h"

#include "os/os.h"

#include "drivers/System.h"
#include "drivers/Console.h"
#include "drivers/HighResolutionTimer.h"

static int terminate(bool success) {
    DBG("----------------------------------------");
    DBG("Finished %s", success ? "successful" : "with failures");
    DBG("========================================\n");
    while (1) {}
}

template<typename T>
int integrationTest() {
    System::init();
    Console::init();
    HighResolutionTimer::init();

    static T test;

    DBG("\n========================================");
    DBG("Integration Test: %s", test.name());
    DBG("----------------------------------------");

    test.init();

    static os::Task<16*1024> testTask("test", 0, [] () {
        test.once();

        if (!test.interactive()) {
            terminate(true);
        }

        while (true) {
            test.update();
            os::delay(0);
        }
    });

    os::startScheduler();

    return 0;
}

#define INTEGRATION_TEST_RUNNER(_class_)    \
int main() {                                \
    return integrationTest<_class_>();      \
}

#define INTEGRATION_TEST_RUNNER_EXPECT(_cond_, _fmt_, ...)  \
if (!(_cond_)) {                                            \
    DBG(_fmt_, ##__VA_ARGS__);                              \
    terminate(false);                                       \
}
