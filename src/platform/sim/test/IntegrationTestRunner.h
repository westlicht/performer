
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

    bool success = true;

    DBG("\n========================================");
    DBG("Integration Test: %s", test.name());
    DBG("----------------------------------------");

    try {
        test.init();
        test.once();

        if (test.interactive()) {
            auto &simulator = sim::Simulator::instance();
            while (!simulator.terminate()) {
                simulator.update();

                try {
                    test.update();
                } catch (ExpectError &e) {
                    simulator.close();
                    success = false;
                }

                simulator.render();
            }
        }
    } catch (ExpectError &e) {
        success = false;
    }

    DBG("----------------------------------------");
    DBG("Finished %s", success ? "successful" : "with failures");
    DBG("========================================\n");

    return success ? 0 : 1;
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
