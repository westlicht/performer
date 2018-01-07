
#include "core/Debug.h"

#include "os/os.h"

#include "drivers/System.h"
#include "drivers/Console.h"
#include "drivers/HighResolutionTimer.h"

template<typename T>
int integrationTest() {
    System::init();
    Console::init();
    HighResolutionTimer::init();

    static T test;

    DBG("Running test '%s' ...", test.name());

    test.init();

    if (!test.interactive()) {
        while(1) {}
    }

    static os::Task<16*1024> testTask("test", 0, [] () {
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
    while (1) {}                                            \
}
