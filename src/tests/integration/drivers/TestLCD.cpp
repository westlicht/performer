#include "IntegrationTest.h"

#include "drivers/LCD.h"

#include "core/gfx/FrameBuffer.h"
#include "core/gfx/Canvas.h"

#include "os/os.h"

class TestLCD : public IntegrationTest {
public:
    TestLCD() :
        IntegrationTest("LCD", true),
        frameBuffer(256, 64, frameBufferData),
        canvas(frameBuffer)
    {}

    void init() {
        lcd.init();
    }

    void update() {
        int frame = os::ticks() / os::time::ms(33); // around 30 fps

        canvas.setColor(0);
        canvas.fill();

        canvas.setColor(0xf);
        canvas.vline(frame % 256, 0, 64);
        canvas.hline(0, frame % 64, 256);

        lcd.draw(frameBuffer.data());
    }

private:
    uint8_t frameBufferData[256*64];
    FrameBuffer8bit frameBuffer;
    Canvas canvas;
    LCD lcd;
};

INTEGRATION_TEST(TestLCD)
