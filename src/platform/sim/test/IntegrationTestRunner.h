
#include "sim/Simulator.h"

#include "core/Debug.h"

#include "os/os.h"

#include <memory>
#include <stdexcept>

class ExpectError : public std::exception {};

template<typename T>
int integrationTestRunner() {
    sdl::Init init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    T test;

    DBG("\n========================================");
    DBG("Integration Test: %s", test.name());
    DBG("----------------------------------------");

    try {
        test.init();
    } catch (ExpectError &e) {
        return 1;
    }

    if (!test.interactive()) {
        DBG("----------------------------------------");
        DBG("Finished");
        DBG("========================================\n");
        return 0;
    }

    auto &simulator = sim::Simulator::instance();
    int result = 0;
    while (!simulator.terminate()) {
        simulator.update();
        try {
            test.update();
        } catch (ExpectError &e) {
            simulator.close();
            result = 1;
        }
        simulator.render();
    }

    return result;
}

#define INTEGRATION_TEST_RUNNER(_class_)        \
int main() {                                    \
    return integrationTestRunner<_class_>();    \
}

#define INTEGRATION_TEST_RUNNER_EXPECT(_cond_, _fmt_, ...)  \
if (!(_cond_)) {                                            \
    DBG(_fmt_, ##__VA_ARGS__);                              \
    throw ExpectError();                                    \
}
