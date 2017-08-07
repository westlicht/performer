#pragma once

#include "ui/Key.h"

#include "model/Model.h"

#include "engine/Engine.h"

struct PageContext {
    KeyState &keyState;
    Model &model;
    Engine &engine;
};
