#pragma once

#include "sim/Simulator.h"

#include <functional>
#include <memory>

class Dio : private sim::TargetInputHandler {
public:
    struct Input {
        typedef std::function<void(bool)> Handler;

        bool get() const { return _value; }
        void setHandler(Handler handler) { _handler = handler; }

    private:
        void set(bool value) {
            if (_handler && value != _value) {
                _handler(value);
            }
            _value = value;
        }

        bool _value = false;
        Handler _handler;

        friend class Dio;
    };

    struct Output {
        void set(bool value) {
            if (_handler) {
                _handler(value);
            }
        }

    private:
        typedef std::function<void(bool)> Handler;

        void setHandler(Handler handler) { _handler = handler; }

        Handler _handler;

        friend class Dio;
    };

    Dio() :
        _simulator(sim::Simulator::instance())
    {
        _simulator.registerTargetInputObserver(this);

        clockOutput.setHandler([this] (int value) {
            _simulator.writeDigitalOutput(0, value);
        });

        resetOutput.setHandler([this] (int value) {
            _simulator.writeDigitalOutput(1, value);
        });
    }

    void init() {}

    Input clockInput;
    Input resetInput;

    Output clockOutput;
    Output resetOutput;

private:
    void writeDigitalInput(int pin, bool value) override {
        switch (pin) {
        case 0: clockInput.set(value); break;
        case 1: resetInput.set(value); break;
        }
    }

    sim::Simulator &_simulator;
};
