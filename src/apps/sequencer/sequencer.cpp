#include "drivers/ADC.h"
#include "drivers/ButtonLedMatrix.h"
#include "drivers/Console.h"
#include "drivers/DAC.h"
#include "drivers/GateOutput.h"
#include "drivers/LCD.h"
#include "drivers/MIDI.h"
#include "drivers/System.h"
#include "drivers/DebugLed.h"
#include "drivers/HighResolutionTimer.h"
#include "drivers/USBH.h"
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
static GateOutput gateOutput;
static MIDI midi;
static USBH usbh;

static Profiler profiler;

static Model model;
static Engine engine(model, clockTimer, adc, dac, gateOutput, midi);
static UI ui(model, engine, lcd, blm);

static os::Task<256> blickTask([] () {
	rcc_periph_clock_enable(RCC_GPIOB);
	DebugLed ledRed(GPIOB, GPIO14);
	bool state = false;
	while (1) {
		ledRed.set(state);
		state = !state;
		os::delay(os::time::ms(500));
	}
}, 0);

static os::Task<1024> blmTask([] () {
    uint32_t lastWakeupTime = os::ticks();
	while (1) {
		blm.process();
		os::delayUntil(lastWakeupTime, 1);
	}
}, 5);

static os::Task<1024> usbhTask([] () {
    uint32_t lastWakeupTime = os::ticks();
	while (1) {
		usbh.process();
		os::delayUntil(lastWakeupTime, 1);
	}
}, 5);

static os::Task<1024> profilerTask([] () {
    while (1) {
        profiler.dump();
        os::delay(os::time::ms(1000));
    }
}, 0);

static os::Task<1024> engineTask([] () {
    uint32_t lastWakeupTime = os::ticks();
    while (1) {
        engine.update();
        os::delayUntil(lastWakeupTime, 1);
    }
}, 4);

static os::Task<1024> uiTask([] () {
    uint32_t lastWakeupTime = os::ticks();
    while (1) {
        ui.update();
        os::delayUntil(lastWakeupTime, 1);
    }
}, 3);

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
    gateOutput.init();
    midi.init();
    usbh.init();

    engine.init();
    ui.init();

	os::startScheduler();
}
