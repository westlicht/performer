#pragma once

#include <cstdint>

class Engine;
class Model;
class MidiOutput;

class MidiOutputEngine {
public:
    MidiOutputEngine(Engine &engine, Model &model);

    void reset();
    void tick(uint32_t tick);

    void sendGate(int trackIndex, bool gate);
    void sendCv(int trackIndex, float cv);

private:
    Engine &_engine;
    Model &_model;
    const MidiOutput &_midiOutput;
};
