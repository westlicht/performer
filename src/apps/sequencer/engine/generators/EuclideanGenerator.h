#pragma once

#include "Config.h"

#include "Generator.h"
#include "Rhythm.h"

#include "core/math/Math.h"

class EuclideanGenerator : public Generator {
public:
    enum class Param {
        Steps,
        Beats,
        Offset,
        Last
    };

    struct Params {
        uint8_t steps = 16;
        uint8_t beats = 4;
        uint8_t offset = 0;
    };

    EuclideanGenerator(SequenceBuilder &builder, Params &params);

    Mode mode() const override { return Mode::Euclidean; }

    int paramCount() const override { return int(Param::Last); }
    const char *paramName(int index) const override;
    void editParam(int index, int value, bool shift) override;
    void printParam(int index, StringBuilder &str) const override;

    void init() override;
    void update() override;

    // steps

    int steps() const { return _params.steps; }
    void setSteps(int steps) { _params.steps = clamp(steps, 1, CONFIG_STEP_COUNT); }

    // beats

    int beats() const { return _params.beats; }
    void setBeats(int beats) { _params.beats = clamp(beats, 1, CONFIG_STEP_COUNT); }

    // offset

    int offset() const { return _params.offset; }
    void setOffset(int offset) { _params.offset = clamp(offset, 0, CONFIG_STEP_COUNT - 1); }

    // pattern

    const Rhythm::Pattern &pattern() const { return _pattern; }

private:
    Params &_params;
    Rhythm::Pattern _pattern;
};
