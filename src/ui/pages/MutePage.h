#pragma once

#include "ui/Page.h"

#include "model/Model.h"

#include "engine/Engine.h"

class MutePage : public Page {
public:
    MutePage(PageManager &pageManager, Model &model, Engine &engine);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    Model &_model;
    Engine &_engine;
};
