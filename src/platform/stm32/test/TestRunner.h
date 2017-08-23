
#include "core/Debug.h"

#include "os/os.h"

#include "drivers/System.h"
#include "drivers/Console.h"
#include "drivers/HighResolutionTimer.h"

class TestRunner {
public:
    template<typename Func>
    static void loop(Func func) {
        while (true) {
            func();
        }
    }

    static void sleep(uint32_t ms) {
        auto end = os::ticks() + os::time::ms(ms);
        while (os::ticks() < end) {}
    }
};

extern void test();

int main() {
    System::init();
    Console::init();
    HighResolutionTimer::init();

    test();

    return 0;
}
