#include "IntegrationTest.h"

#ifdef PLATFORM_STM32
#include "drivers/ShiftRegister.h"
#endif

#include "drivers/GateOutput.h"

#include "os/os.h"

class TestGateOutput : public IntegrationTest {
public:
    TestGateOutput() :
#ifdef PLATFORM_STM32
        _gateOutput(_shiftRegister),
#endif
        _driverTask("driver", 0, 1, [&] () {
#ifdef PLATFORM_STM32
            _shiftRegister.process();
#endif
        })
    {
    }

    void init() override {
#ifdef PLATFORM_STM32
        _shiftRegister.init();
#endif
        _gateOutput.init();
    }

    void update() override {
        _gateOutput.setGates(_gates);
        ++_gates;
        os::delay(os::time::ms(50));
    }

private:
#ifdef PLATFORM_STM32
    ShiftRegister _shiftRegister;
#endif
    GateOutput _gateOutput;
    os::PeriodicTask<1024> _driverTask;
    uint8_t _gates = 0;
};

INTEGRATION_TEST(TestGateOutput, "GateOutput", true)
