
#include "sim/Simulator.h"

#include "core/Debug.h"

#include "os/os.h"

#include <memory>
#include <stdexcept>

class ExpectError : public std::exception {
public:
    // ExpectError() = default;
};

template<typename T>
int integrationTestRunner(const char *name) {
    DBG("Running test '%s' ...", name);

    sdl::Init init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    T test;

    try {
        test.init();
    } catch (ExpectError &e) {
        return 1;
    }

    auto &simulator = sim::Simulator::instance();
    int result = 0;
    while (!simulator.terminate()) {
        simulator.update();
        try {
            test.update();
        } catch (ExpectError &e) {
            result = 1;
        }
        if (test.terminate()) {
            simulator.close();
        }
        simulator.render();
    }

    return result;
}

#define INTEGRATION_TEST_RUNNER(_name_, _class_)    \
int main() {                                        \
    return integrationTestRunner<_class_>(_name_);  \
}

#define INTEGRATION_TEST_RUNNER_EXPECT(_cond_, _fmt_, ...)  \
if (!(_cond_)) {                                            \
    DBG(_fmt_, ##__VA_ARGS__);                              \
    throw ExpectError();                                    \
}
