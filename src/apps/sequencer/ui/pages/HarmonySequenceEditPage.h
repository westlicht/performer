#pragma once

#include "ListPage.h"

#include "ui/StepSelection.h"
#include "ui/model/HarmonySequenceStepListModel.h"
#include "ui/model/HarmonySequenceListModel.h"

#include "core/utils/Container.h"

class HarmonySequenceEditPage : public ListPage {
public:
    HarmonySequenceEditPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;
    virtual void midi(MidiEvent &event) override;

private:
    virtual void drawCell(Canvas &canvas, int row, int column, int x, int y, int w, int h) override;

    void updateSelectedStep();

    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;

    void initSequence();
    void copySequence();
    void pasteSequence();
    void duplicateSequence();

    void quickEdit(int index);

    HarmonySequenceStepListModel _stepListModel;
    HarmonySequenceListModel _sequenceListModel;

    HarmonyStepSelection<16> _stepSelection;
};
