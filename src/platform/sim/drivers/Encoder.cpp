#include "Encoder.h"

Encoder::Encoder() :
    _simulator(sim::Simulator::instance())
{
    _encoder = _simulator.window().createWidget<sim::Encoder>(
        sim::Vector2i(8, 8 + 32),
        sim::Vector2i(64, 64)
    );
    _encoder->setButtonCallback([this] (bool pressed) {
        _events.emplace_back(pressed ? Event::Down : Event::Up);
    });
    _encoder->setValueCallback([this] (int value) {
        if (value > 0) {
            for (int i = 0; i < value; ++i) {
                _events.emplace_back(Event::Right);
            }
        } else if (value < 0) {
            for (int i = 0; i > value; --i) {
                _events.emplace_back(Event::Left);
            }
        }
    });
}

bool Encoder::nextEvent(Event &event) {
    if (_events.empty()) {
        return false;
    }
    event = _events.front();
    _events.pop_front();
    return true;
}
