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
        rampUpHalf,
        rampDownHalf,
        doubleRampUpHalf,
        doubleRampDownHalf,
        ExpUp,
        ExpDown,
        expUpHalf,
        expDownHalf,
        doubleExpUpHalf,
        doubleExpDownHalf,
        LogUp,
        LogDown,
        logUpHalf,
        logDownHalf,
        doubleLogUpHalf,
        doubleLogDownHalf,
        SmoothUp,
        SmoothDown,
        smoothUpHalf,
        smoothDownHalf,
        doubleSmoothUpHalf,
        doubleSmoothDownHalf,
        Triangle,
        Bell,
        ExpDown2x,
        ExpDown3x,
        ExpDown4x,
        Last,
    };

    static Function function(Type type);

    static float eval(Type type, float x);
};

