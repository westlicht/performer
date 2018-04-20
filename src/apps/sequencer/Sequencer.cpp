#include "Config.h"

#include "drivers/Adc.h"
#include "drivers/ButtonLedMatrix.h"
#include "drivers/Console.h"
#include "drivers/Dac.h"
#include "drivers/Dio.h"
#include "drivers/GateOutput.h"
#include "drivers/Lcd.h"
#include "drivers/Midi.h"
#include "drivers/System.h"
#include "drivers/ShiftRegister.h"
#include "drivers/DebugLed.h"
#include "drivers/HighResolutionTimer.h"
#include "drivers/UsbH.h"
#include "drivers/UsbMidi.h"
#include "drivers/ClockTimer.h"
#include "drivers/SdCard.h"

#include "os/os.h"

#include "core/profiler/Profiler.h"
#include "core/fs/Volume.h"

#include "model/Model.h"
#include "model/FileManager.h"
#include "engine/Engine.h"
#include "ui/Ui.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

// There are two RAM regions on the STM32F405:
// - 128kB SRAM at 0x20000000
// - 64kB CCMRAM at 0x10000000
// CCMRAM (core-coupled-memory) may be faster than SRAM but cannot be used for DMA tranfers!
// We use CCMRAM for all the drivers not using DMA, all tasks and some of the application objects (engine, ui).
// This leaves most of the SRAM for the model, which needs a large contiguous block of memory.

static CCMRAM_BSS ClockTimer clockTimer;
static CCMRAM_BSS ShiftRegister shiftRegister;
static CCMRAM_BSS ButtonLedMatrix blm(shiftRegister);
static CCMRAM_BSS Encoder encoder;
static Lcd lcd;
static Adc adc;
static CCMRAM_BSS Dac dac;
static CCMRAM_BSS Dio dio;
static CCMRAM_BSS GateOutput gateOutput(shiftRegister);
static CCMRAM_BSS Midi midi;
static CCMRAM_BSS UsbMidi usbMidi;
static UsbH usbh(usbMidi);
static SdCard sdCard;

static fs::Volume volume(sdCard);

static CCMRAM_BSS Profiler profiler;

static Model model;
static CCMRAM_BSS Engine engine(model, clockTimer, adc, dac, dio, gateOutput, midi, usbMidi);
static CCMRAM_BSS Ui ui(model, engine, lcd, blm, encoder);

static CCMRAM_BSS os::PeriodicTask<1024> driverTask("driver", CONFIG_DRIVER_TASK_PRIORITY, os::time::ms(1), [] () {
    shiftRegister.process();
    blm.process();
    encoder.process();
});

static CCMRAM_BSS os::PeriodicTask<4096> engineTask("engine", CONFIG_ENGINE_TASK_PRIORITY, os::time::ms(1), [] () {
    engine.update();
});

static CCMRAM_BSS os::PeriodicTask<2048> usbhTask("usbh", CONFIG_USBH_TASK_PRIORITY, os::time::ms(1), [] () {
    usbh.process();
});

static os::PeriodicTask<4096> uiTask("ui", CONFIG_UI_TASK_PRIORITY, os::time::ms(1), [] () {
    ui.update();
});

static os::PeriodicTask<1024> fsTask("file", CONFIG_FILE_TASK_PRIORITY, os::time::ms(10), [] () {
    FileManager::processTask();
});

#if CONFIG_ENABLE_PROFILER || CONFIG_ENABLE_TASK_PROFILER
static CCMRAM_BSS os::PeriodicTask<2048> profilerTask("profiler", 0, os::time::ms(5000), [&] () {
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
    sdCard.init();

    model.init();
    engine.init();
    ui.init();

	os::startScheduler();
}
