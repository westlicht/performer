
#include "core/Debug.h"

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
};

extern void test();

int main() {
    System::init();
    Console::init();
    HighResolutionTimer::init();

    test();

    return 0;
}
