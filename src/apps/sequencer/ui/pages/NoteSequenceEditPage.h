#pragma once

#include "BasePage.h"
#include "ContextMenu.h"

#include "ui/StepSelection.h"

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

private:
    static const int StepCount = 16;

    enum class Mode : uint8_t {
        Gate,
        GateVariation,
        Retrigger,
        RetriggerVariation,
        Length,
        LengthVariation,
        Note,
        NoteVariation,
        NoteSlide,
        TrigCondition,
    };

    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::Gate:                return "GATE"; break;
        case Mode::GateVariation:       return "GATE VAR"; break;
        case Mode::Retrigger:           return "RETRIG"; break;
        case Mode::RetriggerVariation:  return "RETRIG VAR"; break;
        case Mode::Length:              return "LENGTH"; break;
        case Mode::LengthVariation:     return "LENGTH VAR"; break;
        case Mode::Note:                return "NOTE"; break;
        case Mode::NoteVariation:       return "NOTE VAR"; break;
        case Mode::NoteSlide:           return "NOTE SLIDE"; break;
        case Mode::TrigCondition:       return "COND"; break;
        }
        return nullptr;
    }

    int stepOffset() const { return _page * StepCount; }

    void updateIdleOutput();
    void drawDetail(Canvas &canvas, const NoteSequence::Step &step);

    void contextAction(int index);
    bool contextActionEnabled(int index) const;
    void initSequence();
    void copySequence();
    void pasteSequence();
    void duplicateSequence();

    ContextMenu _contextMenu;

    Mode _mode = Mode::Gate;
    int _page = 0;
    bool _showDetail;

    StepSelection<CONFIG_STEP_COUNT> _stepSelection;
};
