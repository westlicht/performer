#pragma once

#include "ui/Page.h"
#include "ui/Key.h"

#include "model/Model.h"

#include "engine/Engine.h"

struct PageContext {
    KeyState &keyState;
    Model &model;
    Engine &engine;
};

class BasePage : public Page {
public:
    BasePage(PageManager &manager, PageContext &context) :
        Page(manager),
        _context(context),
        _model(context.model),
        _project(context.model.project()),
        _engine(context.engine)
    {}

protected:
    PageContext &_context;
    Model &_model;
    Project &_project;
    Engine &_engine;
};
