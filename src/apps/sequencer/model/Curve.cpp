#include "Curve.h"

#include <algorithm>

#include <cmath>

static const float Pi = 3.1415926536f;
static const float TwoPi = 2.f * Pi;

static float low(float x) {
    return 0.f;
}

static float high(float x) {
    return 1.f;
}

static float rampUp(float x) {
    return x;
}

static float rampDown(float x) {
    return 1.f - x;
}

static float expUp(float x) {
    return x * x;
}

static float expDown(float x) {
    return (1.f - x) * (1.f - x);
}

static float logUp(float x) {
    return std::sqrt(x);
}

static float logDown(float x) {
    return std::sqrt(1.f - x);
}

static float smoothUp(float x) {
    return x * x * (3.f - 2.f * x);
}

static float smoothDown(float x) {
    return 1.f - x * x * (3.f - 2.f * x);
}

static float rampUpHalf(float x) {
    return x < 0.5f ? rampUp(std::fmod(x * 2.f, 1.f)) : 0.f;
}

static float rampDownHalf(float x) {
    return x < 0.5f ? rampDown(std::fmod(x * 2.f, 1.f)) : 0.f;
}

static float expUpHalf(float x) {
    return x < 0.5f ? expUp(std::fmod(x * 2.f, 1.f)) : 0.f;
}

static float expDownHalf(float x) {
    return x < 0.5f ? expDown(std::fmod(x * 2.f, 1.f)) : 0.f;
}

static float logUpHalf(float x) {
    return x < 0.5f ? logUp(std::fmod(x * 2.f, 1.f)) : 0.f;
}

static float logDownHalf(float x) {
    return x < 0.5f ? logDown(std::fmod(x * 2.f, 1.f)) : 0.f;
}

static float smoothUpHalf(float x) {
    return x < 0.5f ? smoothUp(std::fmod(x * 2.f, 1.f)) : 0.f;
}

static float smoothDownHalf(float x) {
    return x < 0.5f ? smoothDown(std::fmod(x * 2.f, 1.f)) : 0.f;
}

static float triangle(float x) {
    return (x < 0.5f ? x : 1.f - x) * 2.f;
}

static float bell(float x) {
    return 0.5f - 0.5f * std::cos(x * TwoPi);
}

static float stepUp(float x) {
    return x < 0.5f ? 0.f : 1.f;
}

static float stepDown(float x) {
    return x < 0.5f ? 1.f : 0.f;
}

static float expDown2x(float x) {
    return x < 1.f ? expDown(std::fmod(x * 2.f, 1.f)) : 0.f;
}

static float expDown3x(float x) {
    return x < 1.f ? expDown(std::fmod(x * 3.f, 1.f)) : 0.f;
}

static float expDown4x(float x) {
    return x < 1.f ? expDown(std::fmod(x * 4.f, 1.f)) : 0.f;
}

static Curve::Function functions[] = {
        &low,
        &high,
        &rampUp,
        &rampDown,
        &expUp,
        &expDown,
        &logUp,
        &logDown,
        &smoothUp,
        &smoothDown,
        &rampUpHalf,
        &rampDownHalf,
        &expUpHalf,
        &expDownHalf,
        &logUpHalf,
        &logDownHalf,
        &smoothUpHalf,
        &smoothDownHalf,
        &triangle,
        &bell,
        &stepUp,
        &stepDown,
        &expDown2x,
        &expDown3x,
        &expDown4x,
};

Curve::Function Curve::function(Type type) {
    return functions[type];
}

float Curve::eval(Type type, float x) {
    return functions[type](x);
}