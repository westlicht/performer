#pragma once

#include "Config.h"

#include "Generator.h"

#include "core/math/Math.h"

class RandomGenerator : public Generator {
public:
    enum class Param {
        Seed,
        Smooth,
        Bias,
        Scale,
        Last
    };

    RandomGenerator(SequenceBuilder &builder);

    Mode mode() const override { return Mode::Random; }

    int paramCount() const override { return int(Param::Last); }
    const char *paramName(int index) const override;
    void editParam(int index, int value, bool shift) override;
    void printParam(int index, StringBuilder &str) const override;

    void update() override;

    // seed

    int seed() const { return _seed; }
    void setSeed(int seed) { _seed = clamp(seed, 0, 1000); }

    // smooth

    int smooth() const { return _smooth; }
    void setSmooth(int smooth) { _smooth = clamp(smooth, 0, 10); }

    // bias

    int bias() const { return _bias; }
    void setBias(int bias) { _bias = clamp(bias, -10, 10); }

    // scale

    int scale() const { return _scale; }
    void setScale(int scale) { _scale = clamp(scale, 0, 100); }

    // pattern

    const GeneratorPattern &pattern() const { return _pattern; }

private:
    uint16_t _seed = 0;
    uint8_t _smooth = 0;
    int8_t _bias = 0;
    uint8_t _scale = 10;

    GeneratorPattern _pattern;
};
