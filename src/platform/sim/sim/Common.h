#pragma once

#include "sdlpp.hpp"

#include <Eigen/Core>

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include <functional>
#include <cstdint>

namespace sim {

using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Vector4f;
using Eigen::Vector2i;
using Eigen::Vector3i;
using Eigen::Vector4i;

inline Vector2f toFloat(const Vector2i &v) { return Vector2f(float(v.x()), float(v.y())); }
inline Vector2i toInt(const Vector2f &v) { return Vector2i(int(v.x()), int(v.y())); }

static const float PI = 3.141592653589793f;
static const float TWO_PI = 6.283185307179586f;

class Color : public Eigen::Vector4f {
    typedef Eigen::Vector4f Base;
public:
    Color() : Color(0, 0, 0, 0) {}
    Color(const Eigen::Vector4f &color) : Eigen::Vector4f(color) {}
    Color(const Eigen::Vector3f &color, float alpha) : Color(color(0), color(1), color(2), alpha) {}
    Color(const Eigen::Vector3i &color, int alpha) : Color(color.cast<float>() / 255.f, alpha / 255.f) {}
    Color(const Eigen::Vector3f &color) : Color(color, 1.0f) {}
    Color(const Eigen::Vector3i &color) : Color((Vector3f)(color.cast<float>() / 255.f)) {}
    Color(const Eigen::Vector4i &color) : Color((Vector4f)(color.cast<float>() / 255.f)) {}
    Color(float intensity, float alpha) : Color(Vector3f::Constant(intensity), alpha) {}
    Color(int intensity, int alpha) : Color(Vector3i::Constant(intensity), alpha) {}
    Color(float r, float g, float b, float a) : Color(Vector4f(r, g, b, a)) {}
    Color(int r, int g, int b, int a) : Color(Vector4i(r, g, b, a)) {}

    /// Construct a color vector from MatrixBase (needed to play nice with Eigen)
    template <typename Derived> Color(const Eigen::MatrixBase<Derived>& p) : Base(p) {}

    /// Assign a color vector from MatrixBase (needed to play nice with Eigen)
    template <typename Derived> Color &operator=(const Eigen::MatrixBase<Derived>& p) {
        this->Base::operator=(p);
        return *this;
    }

    const float &r() const { return x(); }
          float &r()       { return x(); }

    const float &g() const { return y(); }
          float &g()       { return y(); }

    const float &b() const { return z(); }
          float &b()       { return z(); }

    const float &a() const { return w(); }
          float &a()       { return w(); }

    Color contrastingColor() const {
        float luminance = cwiseProduct(Color(0.299f, 0.587f, 0.144f, 0.f)).sum();
        return Color(luminance < 0.5f ? 1.f : 0.f, 1.f);
    }

    uint32_t rgba() const {
        return (
            std::min(255, int(std::floor(x() * 255))) |
            std::min(255, int(std::floor(y() * 255))) << 8 |
            std::min(255, int(std::floor(z() * 255))) << 16 |
            std::min(255, int(std::floor(w() * 255))) << 24
        );
    }

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace sim
