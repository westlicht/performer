#pragma once

#include "PageContext.h"

#include "ui/Page.h"

#include "model/Model.h"

#include "engine/Engine.h"

#include <bitset>

class TrackPage : public Page {
public:
    TrackPage(PageManager &pageManager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    PageContext &_context;
    Model &_model;
    Engine &_engine;
    Project &_project;

    enum class EditMode {
        Gate,
        Length,
        Note,
    };

    EditMode _editMode = EditMode::Gate;

    std::bitset<CONFIG_STEP_COUNT> _selectedSteps;
};
