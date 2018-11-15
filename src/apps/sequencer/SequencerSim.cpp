#include "SequencerApp.h"

#include "sim/Simulator.h"

#ifdef __EMSCRIPTEN__

#include <emscripten.h>

static SequencerApp *g_app;
static Simulator *g_sim;

static void mainLoop() {
    g_sim->update();
    g_app->update();
    g_sim->render();
    g_sim->delay(1);
}

int main(int argc, char *argv[]) {
    g_sim = &sim::Simulator::instance();

    SequencerApp app;
    g_app = &app;

    // 0 fps means to use requestAnimationFrame; non-0 means to use setTimeout.
    emscripten_set_main_loop(mainLoop, 0, 1);

    return 0;
}

#else // __EMSCRIPTEN__

int main(int argc, char *argv[]) {
    auto &sim = sim::Simulator::instance();

    SequencerApp app;

    while (!sim.terminate()) {
        sim.update();
        app.update();
        sim.render();
    }

    return 0;
}

#endif // __EMSCRIPTEN__
