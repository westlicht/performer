#include "drivers/ADC.h"
#include "drivers/ButtonLedMatrix.h"
#include "drivers/Console.h"
#include "drivers/DAC.h"
#include "drivers/DIO.h"
#include "drivers/GateOutput.h"
#include "drivers/LCD.h"
#include "drivers/MIDI.h"
#include "drivers/System.h"
#include "drivers/DebugLed.h"
#include "drivers/HighResolutionTimer.h"
#include "drivers/USBH.h"
#include "drivers/USBMIDI.h"
#include "drivers/ClockTimer.h"

#include "os/os.h"

#include "core/profiler/Profiler.h"

#include "model/Model.h"
#include "engine/Engine.h"
#include "ui/UI.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

static ClockTimer clockTimer;
static ButtonLedMatrix blm;
static LCD lcd;
static ADC adc;
static DAC dac;
static DIO dio;
static GateOutput gateOutput;
static MIDI midi;
static USBMIDI usbMidi;
static USBH usbh(usbMidi);

static Profiler profiler;

static Model model;
static Engine engine(model, clockTimer, adc, dac, dio, gateOutput, midi, usbMidi);
static UI ui(model, engine, lcd, blm);

static os::Task<256> blickTask("blink", 0, [] () {
	rcc_periph_clock_enable(RCC_GPIOB);
	DebugLed ledRed(GPIOB, GPIO14);
	bool state = false;
	while (1) {
		ledRed.set(state);
		state = !state;
		os::delay(os::time::ms(500));
	}
});

static os::Task<1024> blmTask("blm", 5, [] () {
    uint32_t lastWakeupTime = os::ticks();
	while (1) {
		blm.process();
		os::delayUntil(lastWakeupTime, 1);
	}
});

static os::Task<1024> usbhTask("usbh", 1, [] () {
    uint32_t lastWakeupTime = os::ticks();
	while (1) {
		usbh.process();
		os::delayUntil(lastWakeupTime, 1);
	}
});

static os::Task<1024> profilerTask("profiler", 0, [] () {
    while (1) {
        profiler.dump();
        os::delay(os::time::ms(1000));
    }
});

static os::Task<800> engineTask("engine", 4, [] () {
    uint32_t lastWakeupTime = os::ticks();
    while (1) {
        engine.update();
        os::delayUntil(lastWakeupTime, 1);
    }
});

static os::Task<2048> uiTask("ui", 3, [] () {
    uint32_t lastWakeupTime = os::ticks();
    while (1) {
        ui.update();
        os::delayUntil(lastWakeupTime, 1);
    }
});

template<typename Task>
static void dumpStackUsage(const Task &task) {
    DBG("%s: %d/%d bytes", task.name(), task.stackUsage(), task.stackSize());
}

static os::Task<256> stackCheckTask(nullptr, 0, [&] () {
    while (1) {
        os::delay(os::time::ms(5000));
        DBG("Stack Usage:");
        dumpStackUsage(blmTask);
        dumpStackUsage(usbhTask);
        dumpStackUsage(profilerTask);
        dumpStackUsage(engineTask);
        dumpStackUsage(uiTask);
    }
});

int main(void) {
    System::init();
    Console::init();
    HighResolutionTimer::init();

    profiler.init();

    clockTimer.init();
    blm.init();
    lcd.init();
    adc.init();
    dac.init();
    dio.init();
    gateOutput.init();
    midi.init();
    usbMidi.init();
    usbh.init();

    engine.init();
    ui.init();

	os::startScheduler();
}
