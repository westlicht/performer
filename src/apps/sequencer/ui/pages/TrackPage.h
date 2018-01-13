#pragma once

#include "BasePage.h"

#include <bitset>

class TrackPage : public BasePage {
public:
    TrackPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    enum class EditMode {
        Gate,
        Length,
        Note,
    };

    EditMode _editMode = EditMode::Gate;

    std::bitset<CONFIG_STEP_COUNT> _selectedSteps;
};
