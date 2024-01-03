#pragma once

#include <map>
class Curve {
public:
    typedef float (*Function)(float x);

    enum Type {
        Low,
        High,
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
        RevTriangle,
        Bell,
        RevBell,
        StepUp,
        StepDown,
        ExpDown2x,
        ExpUp2x,
        ExpDown3x,
        ExpUp3x,
        ExpDown4x,
        ExpUp4x,
        Last,
    };

 

    static Function function(Type type);

    static float eval(Type type, float x);

 


};

 
