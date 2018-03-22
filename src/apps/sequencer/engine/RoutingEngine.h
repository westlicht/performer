#pragma once

#include "Config.h"

#include "MIDIPort.h"

#include "model/Model.h"

#include "core/midi/MIDIMessage.h"

#include <array>

#include <cstdint>

class Engine;

class RoutingEngine {
public:
    RoutingEngine(Engine &engine, Model &model);

    void update();

    void receiveMIDI(MIDIPort port, const MIDIMessage &message);

private:
    void updateSources();
    void updateSinks();

    Engine &_engine;
    Model &_model;
    Project &_project;
    Routing &_routing;

    std::array<float, CONFIG_ROUTE_COUNT> _sourceValues;
    std::array<float, CONFIG_ROUTE_COUNT> _shadowValues;
};
