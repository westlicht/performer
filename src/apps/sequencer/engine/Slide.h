#pragma once

namespace Slide {

static float applySlide(float current, float target, int slideTime, float dt) {
    float tau = slideTime / 100.f;
    tau = tau * tau * 2.f;
    float coeff = tau > 0.f ? std::exp(-1.f * dt / tau) : 0.f;
    return target + coeff * (current - target);
}

} // namespace Slide
