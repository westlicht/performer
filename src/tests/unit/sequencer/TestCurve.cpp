#include "UnitTest.h"

#include "core/utils/StringBuilder.h"

#include "apps/sequencer/model/Curve.cpp"
#include "core/gfx/Canvas.cpp"

#ifdef PLATFORM_SIM
#include "libs/stb/stb_image_write.h"
#endif

#include <cstdint>

const int Width = 32;
const int Height = 64;
const int Padding = 4;
float brightness = 1.0;

UNIT_TEST("Curve") {

#ifdef PLATFORM_SIM

    CASE("markdown") {

        auto drawCurve = [] (int index, const char *filename) {
            uint8_t data[Width * Height];
            FrameBuffer8bit framebuffer(Width, Height, data);
            Canvas canvas(framebuffer, brightness);

            canvas.setBlendMode(BlendMode::Set);
            canvas.setColor(Color::None);
            canvas.fill();

            canvas.setBlendMode(BlendMode::Add);
            canvas.setColor(Color::Bright);

            const int Steps = Width * 2;

            for (int i = 0; i < Steps; ++i) {
                float x0 = float(i) / Steps;
                float x1 = float(i + 1) / Steps;
                float y0 = Curve::eval(Curve::Type(index), x0);
                float y1 = Curve::eval(Curve::Type(index), x1);
                canvas.line(
                    x0 * Width,
                    Height - Padding - y0 * (Height - 2 * Padding),
                    x1 * Width,
                    Height - Padding - y1 * (Height - 2 * Padding)
                );
                canvas.line(
                    x0 * Width,
                    Height - Padding - y0 * (Height - 2 * Padding) + 1,
                    x1 * Width,
                    Height - Padding - y1 * (Height - 2 * Padding) + 1
                );
            }

            for (int i = 0; i < Width * Height; ++i) {
                data[i] = std::min(255, int(data[i]) * 0xf);
            }

            stbi_write_png(filename, Width, Height, 1, data, Width * 1);
        };

        FixedStringBuilder<4096> indices("| Index |");
        FixedStringBuilder<4096> separators("| :--- |");
        FixedStringBuilder<4096> shapes("| Shapes |");

        for (int i = 0; i < Curve::Last; ++i) {
            FixedStringBuilder<32> filename("shape-%03d.png", i + 1);
            drawCurve(i, filename);
            indices(" %d |", i + 1);
            separators(" --- |");
            shapes(" <img width=\"%d\" height=\"%d\" style=\"max-width: none\" src=\"images/%s\"/> |", Width, Height, (const char *)(filename));
        }

        DBG("----------------------------------------");
        DBG("");
        DBG("%s", (const char *)(indices));
        DBG("%s", (const char *)(separators));
        DBG("%s", (const char *)(shapes));
        DBG("");
        DBG("----------------------------------------");
    }

#endif // PLATFORM_SIM

}
