
#include "sim/Simulator.h"

#include "drivers/ADC.h"
#include "drivers/ButtonLedMatrix.h"
#include "drivers/ClockTimer.h"
#include "drivers/DAC.h"
#include "drivers/DIO.h"
#include "drivers/Encoder.h"
#include "drivers/GateOutput.h"
#include "drivers/LCD.h"
#include "drivers/Midi.h"
#include "drivers/SDCard.h"
#include "drivers/UsbMidi.h"

#include "core/fs/Volume.h"

#include "model/Model.h"
#include "engine/Engine.h"
#include "ui/UI.h"

#ifdef __EMSCRIPTEN__
# include <emscripten.h>
#endif

struct Environment {
    sim::Simulator &simulator;

    // drivers
    ClockTimer clockTimer;
    ButtonLedMatrix blm;
    LCD lcd;
    ADC adc;
    DAC dac;
    DIO dio;
    Encoder encoder;
    GateOutput gateOutput;
    Midi midi;
    UsbMidi usbMidi;
    SDCard sdcard;

    // filesystem
    fs::Volume volume;

    // application
    Model model;
    Engine engine;
    UI ui;

    Environment() :
        simulator(sim::Simulator::instance()),

        volume(sdcard),

        engine(model, clockTimer, adc, dac, dio, gateOutput, midi, usbMidi),
        ui(model, engine, lcd, blm, encoder)
    {
        model.init();
        engine.init();
        ui.init();
    }

    bool terminate() {
        return simulator.terminate();
    }

    void update() {
        simulator.update();
        engine.update();
        ui.update();
        simulator.render();
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
