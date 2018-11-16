#include "IntegrationTest.h"

#include "drivers/Encoder.h"

#include "os/os.h"

class TestEncoder : public IntegrationTest {
public:
    TestEncoder() :
        _encoderTask("encoder", 0, 1, [&] () {
            _encoder.process();
        })
    {}

    void init() override {
        _encoder.init();
    }

    void update() override {
        Encoder::Event event;
        while (_encoder.nextEvent(event)) {
            switch (event) {
            case Encoder::Left:     DBG("Left");    break;
            case Encoder::Right:    DBG("Right");   break;
            case Encoder::Down:     DBG("Down");    break;
            case Encoder::Up:       DBG("Up");      break;
            }
        }
    }

private:
    Encoder _encoder;
    os::PeriodicTask<1024> _encoderTask;
};

INTEGRATION_TEST(TestEncoder, "Encoder", true)
