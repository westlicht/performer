#pragma once

class Curve {
public:
    typedef float (*Function)(float x);

    enum Type {
        High,
        Low,
        RampUp,
        RampDown,
        ExpUp,
        ExpDown,
        LogUp,
        LogDown,
        SmoothUp,
        SmoothDown,
        Last
    };

    static Function function(Type type);

    static float eval(Type type, float x);
};

