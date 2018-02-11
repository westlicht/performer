#include "Curve.h"

#include <algorithm>

#include <cmath>

static float high(float x) {
    return 1.f;
}

static float low(float x) {
    return 0.f;
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

static Curve::Function functions[] = {
    &high,
    &low,
    &rampUp,
    &rampDown,
    &expUp,
    &expDown,
    &logUp,
    &logDown,
    &smoothUp,
    &smoothDown,
};

Curve::Function Curve::function(Type type) {
    return functions[type];
}

float Curve::eval(Type type, float x) {
    return functions[type](x);
}
