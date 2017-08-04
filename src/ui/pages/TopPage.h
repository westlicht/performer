#pragma once

#include "ui/Page.h"

#include "model/Model.h"

#include "engine/Engine.h"

class TopPage : public Page {
public:
    TopPage(PageManager &pageManager, Model &model, Engine &engine);

    virtual void updateLeds(ButtonLedMatrix &blm) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    Model &_model;
    Engine &_engine;
};
