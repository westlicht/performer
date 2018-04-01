#pragma once

#include "Config.h"

#include "MidiPort.h"

#include "model/Model.h"

#include "core/midi/MidiMessage.h"

#include <array>

#include <cstdint>

class Engine;

class RoutingEngine {
public:
    RoutingEngine(Engine &engine, Model &model);

    void update();

    void receiveMidi(MidiPort port, const MidiMessage &message);

private:
    void updateSources();
    void updateSinks();

    Engine &_engine;
    Routing &_routing;

    std::array<float, CONFIG_ROUTE_COUNT> _sourceValues;
};
