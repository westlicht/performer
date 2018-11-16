#include "sim/Simulator.h"
#include "sim/frontend/Frontend.h"

#include "drivers/HighResolutionTimer.h"

#include "core/Debug.h"

#include "os/os.h"

#include <memory>
#include <stdexcept>

class ExpectError : public std::exception {};

template<typename T>
int integrationTestRunner(const char *name, bool interactive) {

    DBG("\n========================================");
    DBG("Integration Test: %s", name);
    DBG("----------------------------------------");

    std::unique_ptr<T> test;

    HighResolutionTimer::init();

    bool success = true;

    sim::Simulator sim({
        .create = [&] () {
            test.reset(new T());

            test->init();
            test->once();

            // if (interactive) {
            //     sim.close();
            // }
        },
        .destroy = [&] () {
            test.reset();
        },
        .update = [&] () {
            test->update();
        }
    });

    sim::Frontend frontend(sim);

    try {
        frontend.run();
    } catch (ExpectError &e) {
        frontend.close();
        success = false;
    }

    DBG("----------------------------------------");
    DBG("Finished %s", success ? "successful" : "with failures");
    DBG("========================================\n");

    return success ? 0 : 1;
}

#define INTEGRATION_TEST_RUNNER(_class_, _name_, _interactive_)     \
int main() {                                                        \
    return integrationTestRunner<_class_>(_name_, _interactive_);   \
}

#define INTEGRATION_TEST_RUNNER_EXPECT(_cond_, _fmt_, ...)  \
if (!(_cond_)) {                                            \
    DBG(_fmt_, ##__VA_ARGS__);                              \
    throw ExpectError();                                    \
}

#define CURRENT_TIME() HighResolutionTimer::us()
