#pragma once

class Curve {
public:
    typedef float (*Function)(float x);

    enum Type {
        Low,
        High,
        RampUp,
        RampDown,
        ExpUp,
        ExpDown,
        LogUp,
        LogDown,
        SmoothUp,
        SmoothDown,
        RampUpHalf,
        RampDownHalf,
        ExpUpHalf,
        ExpDownHalf,
        LogUpHalf,
        LogDownHalf,
        SmoothUpHalf,
        SmoothDownHalf,
        Triangle,
        Bell,
        StepUp,
        StepDown,
        ExpDown2x,
        ExpDown3x,
        ExpDown4x,
        Last
    };

    static Function function(Type type);

    static float eval(Type type, float x);
};
