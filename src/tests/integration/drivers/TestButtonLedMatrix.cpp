#include "IntegrationTest.h"

#ifdef PLATFORM_STM32
#include "drivers/ShiftRegister.h"
#endif

#include "drivers/ButtonLedMatrix.h"

#include "os/os.h"

#include <array>

class TestButtonLedMatrix : public IntegrationTest {
public:
    TestButtonLedMatrix() :
#ifdef PLATFORM_STM32
        _blm(_shiftRegister),
#endif
        _driverTask("driver", 0, 1, [&] () {
#ifdef PLATFORM_STM32
            _shiftRegister.process();
#endif
            _blm.process();
        })
    {
        _leds.fill(0);
    }

    void init() override {
#ifdef PLATFORM_STM32
        _shiftRegister.init();
#endif
        _blm.init();
    }

    void update() override {
        ButtonLedMatrix::Event event;
        while (_blm.nextEvent(event)) {
            switch (event.action()) {
            case ButtonLedMatrix::Event::KeyDown:
                DBG("KeyDown(key=%d)", event.value());
                if (event.value() < int(_leds.size())) {
                    int state = _leds[event.value()];
                    state = (state + 1) % 4;
                    _leds[event.value()] = state;
                    _blm.setLed(event.value(), (state == 1 || state == 3) ? 0xff : 0, (state == 2 || state == 3) ? 0xff : 0);
                }
                break;
            case ButtonLedMatrix::Event::KeyUp:
                DBG("KeyUp(key=%d)", event.value());
                break;
            }
        }
    }

private:
#ifdef PLATFORM_STM32
    ShiftRegister _shiftRegister;
#endif
    ButtonLedMatrix _blm;
    os::PeriodicTask<1024> _driverTask;
    std::array<int, ButtonLedMatrix::Rows * ButtonLedMatrix::ColsLed> _leds;
};

INTEGRATION_TEST(TestButtonLedMatrix, "ButtonLedMatrix", true)
