#include "SystemConfig.h"

#include "drivers/ADC.h"
#include "drivers/ButtonLedMatrix.h"
#include "drivers/Console.h"
#include "drivers/DAC.h"
#include "drivers/DIO.h"
#include "drivers/GateOutput.h"
#include "drivers/LCD.h"
#include "drivers/Midi.h"
#include "drivers/System.h"
#include "drivers/ShiftRegister.h"
#include "drivers/DebugLed.h"
#include "drivers/HighResolutionTimer.h"
#include "drivers/UsbH.h"
#include "drivers/UsbMidi.h"
#include "drivers/ClockTimer.h"

#include "os/os.h"

#include "core/profiler/Profiler.h"

#include "model/Model.h"
#include "engine/Engine.h"
#include "ui/UI.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

static ClockTimer clockTimer;
static ShiftRegister shiftRegister;
static ButtonLedMatrix blm(shiftRegister);
static Encoder encoder;
static LCD lcd;
static ADC adc;
static DAC dac;
static DIO dio;
static GateOutput gateOutput(shiftRegister);
static Midi midi;
static UsbMidi usbMidi;
static UsbH usbh(usbMidi);

static Profiler profiler;

static Model model;
static Engine engine(model, clockTimer, adc, dac, dio, gateOutput, midi, usbMidi);
static UI ui(model, engine, lcd, blm, encoder);

static os::PeriodicTask<1024> driverTask("driver", 5, os::time::ms(1), [] () {
    shiftRegister.process();
    blm.process();
    encoder.process();
});

static os::PeriodicTask<2048> usbhTask("usbh", 1, os::time::ms(1), [] () {
    usbh.process();
});

static os::PeriodicTask<4096> engineTask("engine", 4, os::time::ms(1), [] () {
    engine.update();
});

static os::PeriodicTask<4096> uiTask("ui", 3, os::time::ms(1), [] () {
    ui.update();
});

#if CONFIG_ENABLE_PROFILER || CONFIG_ENABLE_TASK_PROFILER
static os::PeriodicTask<2048> profilerTask("profiler", 0, os::time::ms(5000), [&] () {
#if CONFIG_ENABLE_PROFILER
    profiler.dump();
#endif // CONFIG_ENABLE_PROFILE
#if CONFIG_ENABLE_TASK_PROFILER
    os::TaskProfiler::dump();
#endif // CONFIG_ENABLE_TASK_PROFILER
});
#endif // CONFIG_ENABLE_PROFILER || CONFIG_ENABLE_TASK_PROFILER

int main(void) {
    System::init();
    Console::init();
    HighResolutionTimer::init();

    profiler.init();

    shiftRegister.init();
    clockTimer.init();
    blm.init();
    encoder.init();
    lcd.init();
    adc.init();
    dac.init();
    dio.init();
    gateOutput.init();
    midi.init();
    usbMidi.init();
    usbh.init();
    usbh.powerOn();

    model.init();
    engine.init();
    ui.init();

	os::startScheduler();
}
