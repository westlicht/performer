#pragma once

#include "BasePage.h"

#include "ui/StepSelection.h"

class CurveSequenceEditPage : public BasePage {
public:
    CurveSequenceEditPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    typedef CurveSequence::Layer Layer;

    static const int StepCount = 16;

    int stepOffset() const { return _page * StepCount; }

    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;

    void initSequence();
    void copySequence();
    void pasteSequence();
    void duplicateSequence();

    ContextMenu _contextMenu;

    Layer _layer = Layer::Shape;
    int _page = 0;

    StepSelection<CONFIG_STEP_COUNT> _stepSelection;
};
