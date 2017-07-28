
#include "sim/Simulator.h"

#include "drivers/ClockTimer.h"
#include "drivers/ButtonLedMatrix.h"
#include "drivers/LCD.h"
#include "drivers/ADC.h"
#include "drivers/DAC.h"
#include "drivers/GateOutput.h"
#include "drivers/MIDI.h"

#include "engine/Engine.h"
#include "ui/UI.h"

#ifdef __EMSCRIPTEN__
# include <emscripten.h>
#endif

struct Environment {
    sim::Simulator sim;
    ClockTimer clockTimer;
    ButtonLedMatrix blm;
    LCD lcd;
    ADC adc;
    DAC dac;
    GateOutput gateOutput;
    MIDI midi;

    Engine engine;
    UI ui;

    Environment() :
        clockTimer(sim),
        blm(sim),
        lcd(sim),
        adc(sim),
        dac(sim),
        gateOutput(sim),
        midi(sim),

        engine(clockTimer, adc, dac, gateOutput, midi),
        ui(engine, lcd, blm)
    {
        engine.init();
        ui.init();
    }

    bool terminate() {
        return sim.terminate();
    }

    void update() {
        sim.update();
        engine.update();
        ui.update();
        sim.render();
    }

};

#ifdef __EMSCRIPTEN__
static Environment *g_env;
static void mainLoop() {
    g_env->update();
}
#endif // __EMSCRIPTEN__

int main(int argc, char *argv[]) {
    sdl::Init init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    Environment env;
  
#ifdef __EMSCRIPTEN__
    // 0 fps means to use requestAnimationFrame; non-0 means to use setTimeout.
    g_env = &env;
    emscripten_set_main_loop(mainLoop, 0, 1);
#else // __EMSCRIPTEN__
    while (!env.terminate()) {
        env.update();
    }
#endif // __EMSCRIPTEN__

    return 0;
}
