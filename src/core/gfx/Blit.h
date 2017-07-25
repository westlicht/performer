#pragma once

#include <cstdint>

namespace blit {
    struct set {
        void operator()(FrameBuffer8bit &frameBuffer, int x, int y, uint8_t color) {
            frameBuffer(x, y) = color;
        }
    };
    struct add {
        void operator()(FrameBuffer8bit &frameBuffer, int x, int y, uint8_t color) {
            // frameBuffer(x, y) = std::min(0xff, int(frameBuffer(x, y)) + color);
            frameBuffer(x, y) += color;
        }
    };
    struct sub {
        void operator()(FrameBuffer8bit &frameBuffer, int x, int y, uint8_t color) {
            // frameBuffer(x, y) = std::max(0x00, int(frameBuffer(x, y)) - color);
            frameBuffer(x, y) -= std::min(frameBuffer(x, y), color);
        }
    };
};
