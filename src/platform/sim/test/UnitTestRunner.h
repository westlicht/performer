#include "drivers/HighResolutionTimer.h"

#include "core/Debug.h"

namespace test {

    bool run(const char *name);

} // namespace test

#define UNIT_TEST_RUNNER_PRINTF(_fmt_, ...) \
    dbg_printf(_fmt_, ##__VA_ARGS__)

#define UNIT_TEST_RUNNER(_name_)        \
int main() {                            \
    HighResolutionTimer::init();        \
    return test::run(_name_) ? 0 : 1;   \
}                                       \

#define CURRENT_TIME() HighResolutionTimer::us()
