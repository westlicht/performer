
#include "sim/Simulator.h"

#include "core/Debug.h"

#include "os/os.h"

#include <memory>

class TestRunner {
public:
    template<typename Func>
    static void loop(Func func) {
        auto &simulator = sim::Simulator::instance();
        while (!simulator.terminate()) {
            simulator.update();
            func();
            simulator.render();
        }
    }

    static void sleep(uint32_t ms) {
        auto &simulator = sim::Simulator::instance();
        auto end = os::ticks() + os::time::ms(ms);
        while (!simulator.terminate() && os::ticks() < end) {
            simulator.update();
            simulator.render();
        }
    }
};

extern void test();

int main() {
    sdl::Init init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    test();

    return 0;
}
