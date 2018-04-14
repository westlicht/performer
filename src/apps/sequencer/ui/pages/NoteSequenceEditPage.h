#pragma once

#include "BasePage.h"

#include "ui/StepSelection.h"

#include "engine/generators/SequenceBuilder.h"

#include "core/utils/Container.h"

class NoteSequenceEditPage : public BasePage {
public:
    NoteSequenceEditPage(PageManager &manager, PageContext &context);

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
    typedef NoteSequence::Layer Layer;

    static const int StepCount = 16;

    int stepOffset() const { return _page * StepCount; }

    void updateIdleOutput();
    void drawDetail(Canvas &canvas, const NoteSequence::Step &step);

    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;
    void initSequence();
    void copySequence();
    void pasteSequence();
    void duplicateSequence();
    void generateSequence();

    Layer _layer = Layer::Gate;
    int _page = 0;
    bool _showDetail;
    uint32_t _showDetailTicks;

    StepSelection<CONFIG_STEP_COUNT> _stepSelection;

    Container<NoteSequenceBuilder> _builderContainer;
};
