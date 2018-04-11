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

    EuclideanGenerator(SequenceBuilder &builder);

    Mode mode() const override { return Mode::Euclidean; }

    int paramCount() const override;
    const char *paramName(int index) const override;
    void editParam(int index, int value, bool shift) override;
    void printParam(int index, StringBuilder &str) const override;

    void update() override;

    // steps

    int steps() const { return _steps; }
    void setSteps(int steps) { _steps = clamp(steps, 1, CONFIG_STEP_COUNT); }

    // beats

    int beats() const { return _beats; }
    void setBeats(int beats) { _beats = clamp(beats, 1, CONFIG_STEP_COUNT); }

    // offset

    int offset() const { return _offset; }
    void setOffset(int offset) { _offset = clamp(offset, 0, CONFIG_STEP_COUNT - 1); }

    // pattern

    const Rhythm::Pattern &pattern() const { return _pattern; }

private:
    int _steps = 16;
    int _beats = 4;
    int _offset = 0;

    Rhythm::Pattern _pattern;
};
