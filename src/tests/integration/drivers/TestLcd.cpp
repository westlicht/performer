#include "IntegrationTest.h"

#include "drivers/Lcd.h"

#include "core/gfx/FrameBuffer.h"
#include "core/gfx/Canvas.h"
#include "core/utils/MovingAverage.h"

#include "os/os.h"

class TestLcd : public IntegrationTest {
public:
    TestLcd() :
        frameBuffer(256, 64, frameBufferData),
        canvas(frameBuffer, brightness)
    {}

    void init() override {
        lcd.init();
        timer.reset();
    }

    void update() override {
        int frame = os::ticks() / os::time::ms(33); // around 30 fps

        frameInterval.push(timer.elapsed());

        if (frame % 30 == 0 && frame != lastFrame) {
            DBG("Refresh rate = %.1f fps", 1000000.f / frameInterval());
        }
        lastFrame = frame;

        timer.reset();

        canvas.setColor(Color::None);
        canvas.fill();

        canvas.setColor(Color::Bright);
        canvas.vline(frame % 256, 0, 64);
        canvas.hline(0, frame % 64, 256);

        lcd.draw(frameBuffer.data());
    }

private:
    uint8_t frameBufferData[256*64];
    FrameBuffer8bit frameBuffer;
    Canvas canvas;
    Lcd lcd;
    Timer timer;
    MovingAverage<uint32_t, 10> frameInterval;
    int lastFrame = -1;
    float brightness = 1.0;
};

INTEGRATION_TEST(TestLcd, "Lcd", true)
