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

#include "../hwconfig/HardwareConfig.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

// There are two RAM regions on the STM32F405:
// - 128kB SRAM at 0x20000000
// - 64kB CCMRAM at 0x10000000
// CCMRAM (core-coupled-memory) may be faster than SRAM but cannot be used for DMA transfers!
// We use CCMRAM for all the drivers not using DMA, all tasks and some of the application objects (engine, ui).
// This leaves most of the SRAM for the model, which needs a large contiguous block of memory.

extern "C" {
__attribute__ ((section(".version")))
struct {
    uint32_t magic = CONFIG_VERSION_MAGIC;
    char name[24] = CONFIG_VERSION_NAME;
    uint8_t major = CONFIG_VERSION_MAJOR;
    uint8_t minor = CONFIG_VERSION_MINOR;
    uint16_t revision = CONFIG_VERSION_REVISION;
} version_tag;
} // extern "C"

inline Dac::Type getDacType() {
    switch (HardwareConfig::dacType()) {
    case DacType::DAC8568C: return Dac::Type::DAC8568C;
    case DacType::DAC8568A: return Dac::Type::DAC8568A;
    default: return Dac::Type::DAC8568C;
    }
}

static CCMRAM_BSS ClockTimer clockTimer;
static CCMRAM_BSS ShiftRegister shiftRegister;
static CCMRAM_BSS ButtonLedMatrix blm(shiftRegister, HardwareConfig::invertLeds());
static CCMRAM_BSS Encoder encoder(HardwareConfig::reverseEncoder());
static Lcd lcd;
static Adc adc;
static CCMRAM_BSS Dac dac(getDacType());
static CCMRAM_BSS Dio dio;
static CCMRAM_BSS GateOutput gateOutput(shiftRegister);
static CCMRAM_BSS Midi midi;
static CCMRAM_BSS UsbMidi usbMidi;
static UsbH usbh(usbMidi);
static SdCard sdCard;

static fs::Volume volume(sdCard);

static CCMRAM_BSS uint8_t midiMessagePayloadPool[32];

static CCMRAM_BSS Profiler profiler;

static Model model;
static CCMRAM_BSS Engine engine(model, clockTimer, adc, dac, dio, gateOutput, midi, usbMidi);
static CCMRAM_BSS Ui ui(model, engine, lcd, blm, encoder, model.settings());


static constexpr uint32_t TaskAliveCount = 4;
static constexpr uint32_t TaskAliveMask = (1 << TaskAliveCount) - 1;
static uint32_t taskAliveState;

static void taskAlive(uint32_t task) {
    os::InterruptLock lock;
    taskAliveState |= (1 << task);
    if ((taskAliveState & TaskAliveMask) == TaskAliveMask) {
        System::resetWatchdog();
        taskAliveState = 0;
    }
}

static CCMRAM_BSS os::PeriodicTask<CONFIG_DRIVER_TASK_STACK_SIZE> driverTask("driver", CONFIG_DRIVER_TASK_PRIORITY, os::time::ms(1), [] () {
    shiftRegister.process();
    blm.process();
    encoder.process();
    taskAlive(0);
});

static CCMRAM_BSS os::PeriodicTask<CONFIG_ENGINE_TASK_STACK_SIZE> engineTask("engine", CONFIG_ENGINE_TASK_PRIORITY, os::time::ms(1), [] () {
    engine.update();
    taskAlive(1);
});

static CCMRAM_BSS os::PeriodicTask<CONFIG_USBH_TASK_STACK_SIZE> usbhTask("usbh", CONFIG_USBH_TASK_PRIORITY, os::time::ms(1), [] () {
    usbh.process();
    taskAlive(2);
});

static CCMRAM_BSS os::PeriodicTask<CONFIG_UI_TASK_STACK_SIZE> uiTask("ui", CONFIG_UI_TASK_PRIORITY, os::time::ms(1), [] () {
    ui.update();
    taskAlive(3);
});

static os::PeriodicTask<CONFIG_FILE_TASK_STACK_SIZE> fsTask("file", CONFIG_FILE_TASK_PRIORITY, os::time::ms(10), [] () {
    FileManager::processTask();
    // no task alive handling because processTask() can take a long time to complete
});

#if CONFIG_ENABLE_PROFILER || CONFIG_ENABLE_TASK_PROFILER
static CCMRAM_BSS os::PeriodicTask<CONFIG_PROFILER_TASK_STACK_SIZE> profilerTask("profiler", 0, os::time::ms(5000), [&] () {
#if CONFIG_ENABLE_PROFILER
    profiler.dump();
#endif // CONFIG_ENABLE_PROFILE
#if CONFIG_ENABLE_TASK_PROFILER
    os::TaskProfiler::dump();
#endif // CONFIG_ENABLE_TASK_PROFILER
});
#endif // CONFIG_ENABLE_PROFILER || CONFIG_ENABLE_TASK_PROFILER

static void assert_handler(const char *filename, int line, const char *msg) {
    ui.showAssert(filename, line, msg);
    // keep watchdog satisfied but reset on encoder down
    while (1) {
        System::resetWatchdog();
        encoder.process();
        Encoder::Event event;
        if (encoder.nextEvent(event) && event == Encoder::Event::Down) {
            System::reset();
            while (1) {}
        }
    }
}

int main(void) {
    System::init();
    System::startWatchdog(1000);
    Console::init();
    HighResolutionTimer::init();

    MidiMessage::setPayloadPool(midiMessagePayloadPool, sizeof(midiMessagePayloadPool));

    dbg_set_assert_handler(&assert_handler);

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
    model.settings().readFromFlash();

    engine.init();
    ui.init();

    System::resetWatchdog();

	os::startScheduler();
}
