#pragma once

#include "ui/MessageManager.h"
#include "ui/Page.h"
#include "ui/PageManager.h"
#include "ui/Key.h"

#include "model/Model.h"

#include "engine/Engine.h"

#include <cstdint>

struct PageContext {
    MessageManager &messageManager;
    KeyState &keyState;
    Model &model;
    Engine &engine;
};

class BasePage : public Page {
public:
    BasePage(PageManager &manager, PageContext &context) :
        Page(manager),
        _context(context),
        _keyState(context.keyState),
        _model(context.model),
        _project(context.model.project()),
        _engine(context.engine)
    {}

protected:
    void showMessage(const char *text, uint32_t duration = 1000) {
        _context.messageManager.showMessage(text, duration);
    }

    PageContext &_context;
    KeyState &_keyState;
    Model &_model;
    Project &_project;
    Engine &_engine;
};
