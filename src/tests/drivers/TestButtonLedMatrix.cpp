#include "test/TestRunner.h"

#include "drivers/ButtonLedMatrix.h"

#include <array>

void test() {
    ButtonLedMatrix blm;

    blm.init();

    std::array<int, ButtonLedMatrix::Rows * ButtonLedMatrix::ColsLed> leds;

    TestRunner::loop([&] () {
        ButtonLedMatrix::Event event;
        while (blm.nextEvent(event)) {
            switch (event.action()) {
            case ButtonLedMatrix::KeyDown:
                DBG("KeyDown(key=%d)", event.value());
                if (event.value() < int(leds.size())) {
                    int state = leds[event.value()];
                    state = (state + 1) % 4;
                    leds[event.value()] = state;
                    blm.setLed(event.value(), (state == 1 || state == 3) ? 0xff : 0, (state == 2 || state == 3) ? 0xff : 0);
                }
                break;
            case ButtonLedMatrix::KeyUp:
                DBG("KeyUp(key=%d)", event.value());
                break;
            case ButtonLedMatrix::Encoder:
                DBG("Encoder(value=%d)", event.value());
                break;
            }
        }
    });
}
