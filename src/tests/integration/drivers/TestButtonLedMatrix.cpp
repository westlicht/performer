#include "test/TestRunner.h"

#include "drivers/ShiftRegister.h"
#include "drivers/ButtonLedMatrix.h"

#include "os/os.h"

#include <array>

class TestButtonLedMatrix : public Test {
public:
    TestButtonLedMatrix() :
        _blm(_shiftRegister),
        _driverTask("driver", 0, 1, [&] () {
            _shiftRegister.process();
            _blm.process();
        })
    {
        _leds.fill(0);
    }

    void init() {
        _shiftRegister.init();
        _blm.init();
    }

    void update() {
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
    ShiftRegister _shiftRegister;
    ButtonLedMatrix _blm;
    os::PeriodicTask<1024> _driverTask;
    std::array<int, ButtonLedMatrix::Rows * ButtonLedMatrix::ColsLed> _leds;
    int _led = 0;
};

TEST(TestButtonLedMatrix)
