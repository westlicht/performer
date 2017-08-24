#include "test/TestRunner.h"

#include "drivers/ButtonLedMatrix.h"

#include "os/os.h"

#include <array>

class TestButtonLedMatrix : public Test {
public:
    TestButtonLedMatrix() :
        blmTask("blm", 0, [&] () {
            uint32_t lastWakeupTime = os::ticks();
            while (1) {
                blm.process();
                os::delayUntil(lastWakeupTime, 1);
            }
        })
    {
        leds.fill(0);
    }

    void init() {
        blm.init();
    }

    void update() {
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
    }

private:
    ButtonLedMatrix blm;
    os::Task<1024> blmTask;
    std::array<int, ButtonLedMatrix::Rows * ButtonLedMatrix::ColsLed> leds;
};

TEST(TestButtonLedMatrix)
