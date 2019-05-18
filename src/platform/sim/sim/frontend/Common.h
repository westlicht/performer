#pragma once

#include "Vector.h"

#include <SDL.h>

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include <functional>
#include <cstdint>
#include <cmath>

namespace sim {

static const float PI = 3.141592653589793f;
static const float TWO_PI = 6.283185307179586f;

class Color : public Vector4f {
public:
    Color() : Vector4f(0.f, 0.f, 0.f, 0.f) {}
    explicit Color(const Vector4f &color) : Vector4f(color) {}
    explicit Color(const Vector3f &color, float alpha) : Color(color[0], color[1], color[2], alpha) {}
    explicit Color(const Vector3i &color, int alpha) : Color(Vector3f(color) / 255.f, alpha / 255.f) {}
    explicit Color(const Vector3f &color) : Color(color, 1.0f) {}
    explicit Color(const Vector3i &color) : Color((Vector3f(color) / 255.f)) {}
    explicit Color(const Vector4i &color) : Color((Vector4f(color) / 255.f)) {}
    explicit Color(float intensity, float alpha) : Color(Vector3f(intensity), alpha) {}
    explicit Color(int intensity, int alpha) : Color(Vector3i(intensity), alpha) {}
    explicit Color(float r, float g, float b, float a) : Color(Vector4f(r, g, b, a)) {}
    explicit Color(int r, int g, int b, int a) : Color(Vector4i(r, g, b, a)) {}

    const float &r() const { return x(); }
          float &r()       { return x(); }

    const float &g() const { return y(); }
          float &g()       { return y(); }

    const float &b() const { return z(); }
          float &b()       { return z(); }

    const float &a() const { return w(); }
          float &a()       { return w(); }

    uint32_t rgba() const {
        return (
            std::min(255, int(std::floor(x() * 255))) |
            std::min(255, int(std::floor(y() * 255))) << 8 |
            std::min(255, int(std::floor(z() * 255))) << 16 |
            std::min(255, int(std::floor(w() * 255))) << 24
        );
    }
};

} // namespace sim
