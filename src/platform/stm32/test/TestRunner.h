
#include "core/Debug.h"

#include "os/os.h"

#include "drivers/System.h"
#include "drivers/Console.h"
#include "drivers/HighResolutionTimer.h"

class Test {
public:
    virtual void init() {}
    virtual void update() {}
};

template<typename T>
int runTest() {
    System::init();
    Console::init();
    HighResolutionTimer::init();

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

#define TEST(_test_)            \
int main() {                    \
    return runTest<_test_>();   \
}
