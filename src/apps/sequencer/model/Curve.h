#pragma once

class Curve {
public:
    typedef float (*Function)(float x);

    enum Type {
        Low,
        High,
        StepUp,
        StepDown,
        RampUp,
        RampDown,
        ExpUp,
        ExpDown,
        LogUp,
        LogDown,
        SmoothUp,
        SmoothDown,
        Triangle,
        Bell,
        ExpDown2x,
        ExpDown3x,
        ExpDown4x,
        Last
    };

    static Function function(Type type);

    static float eval(Type type, float x);
};

