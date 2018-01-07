
#include "core/Debug.h"

#include "os/os.h"

#include "drivers/System.h"
#include "drivers/Console.h"
#include "drivers/HighResolutionTimer.h"

template<typename T>
int integrationTest(const char *name) {
    System::init();
    Console::init();
    HighResolutionTimer::init();

    DBG("Running test '%s' ...", name);

    static T test;

    static os::Task<16*1024> testTask("test", 0, [] () {
        while (true) {
            test.update();
            os::delay(0);
        }
    });

    test.init();

    os::startScheduler();

    return 0;
}

#define INTEGRATION_TEST_RUNNER(_name_, _class_)    \
int main() {                                        \
    return integrationTest<_class_>(_name_);        \
}

#define INTEGRATION_TEST_RUNNER_EXPECT(_cond_, _fmt_, ...)  \
if (!(_cond_)) {                                            \
    DBG(_fmt_, ##__VA_ARGS__);                              \
    while (1) {}                                            \
}
