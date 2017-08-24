
#include "sim/Simulator.h"

#include "core/Debug.h"

#include "os/os.h"

#include <memory>

class Test {
public:
    virtual void init() {}
    virtual void update() {}
};

template<typename T>
int runTest() {
    sdl::Init init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    T test;

    test.init();

    auto &simulator = sim::Simulator::instance();
    while (!simulator.terminate()) {
        simulator.update();
        test.update();
        simulator.render();
    }

    return 0;
}

#define TEST(_test_)            \
int main() {                    \
    return runTest<_test_>();   \
}
