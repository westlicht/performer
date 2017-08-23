#include "DIO.h"

#include "sim/widgets/Button.h"

#include <functional>

using namespace sim;

class ClockSource {
public:
    ClockSource(Simulator &simulator, std::function<void()> handler) :
        _simulator(simulator),
        _handler(handler)
    {
        _simulator.addUpdateCallback([this] () { update(); });
    }

    void toggle() {
        _active = !_active;
        if (_active) {
            _lastTicks = _simulator.ticks();
        }
    }

    void update() {
        if (_active) {
            double currentTicks = _simulator.ticks();
            double interval = clockInterval() * 1000.0;
            while (_lastTicks <= currentTicks + interval) {
                if (_handler) {
                    _handler();
                }
                _lastTicks += interval;
            }
        }
    }

private:
    double clockInterval() {
        return 60.0 / (_bpm * _ppqn);
    }

    Simulator &_simulator;
    std::function<void()> _handler;

    bool _active = false;
    int _ppqn = 16;
    double _bpm = 120.0;

    double _lastTicks;
};

DIO::DIO() :
    _simulator(sim::Simulator::instance())
{
    _clockSource.reset(new ClockSource(_simulator, [this] () {
        clockInput.set(true);
        clockInput.set(false);
    }));

    auto button = _simulator.window().createWidget<sim::Button>(
        sim::Vector2i(600,100),
        sim::Vector2i(20, 20),
        SDLK_F10
    );
    button->setCallback([this] (bool pressed) {
        if (pressed) {
            _clockSource->toggle();
        }
    });

    button = _simulator.window().createWidget<sim::Button>(
        sim::Vector2i(600,130),
        sim::Vector2i(20, 20),
        SDLK_F11
    );
    button->setCallback([this] (bool pressed) {
        resetInput.set(pressed);
    });
}

DIO::~DIO() {
}

void DIO::init() {
}
