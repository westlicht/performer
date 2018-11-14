#pragma once

#include "ui/MessageManager.h"
#include "ui/Page.h"
#include "ui/PageManager.h"
#include "ui/Key.h"
#include "ui/pages/ContextMenu.h"

#include "model/Model.h"

#include "engine/Engine.h"

#include <cstdint>

struct PageContext {
    MessageManager &messageManager;
    KeyState &pageKeyState;
    KeyState &globalKeyState;
    Model &model;
    Engine &engine;

    ContextMenu contextMenu;
};

class BasePage : public Page {
public:
    BasePage(PageManager &manager, PageContext &context);

protected:
    void showMessage(const char *text, uint32_t duration = 1000);
    void showContextMenu(const ContextMenu &contextMenu);

    const KeyState &pageKeyState() const { return _context.pageKeyState; }
    const KeyState &globalKeyState() const { return _context.globalKeyState; }

    PageContext &_context;
    Model &_model;
    Project &_project;
    Engine &_engine;
};
