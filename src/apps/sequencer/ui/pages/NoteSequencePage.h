#pragma once

#include "BasePage.h"

#include "ui/StepSelection.h"

class NoteSequencePage : public BasePage {
public:
    NoteSequencePage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    enum class Mode : uint8_t {
        Gate,
        GateVariation,
        Length,
        LengthVariation,
        Note,
        NoteVariation,
    };

    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::Gate:            return "GATE"; break;
        case Mode::GateVariation:   return "GATE VAR"; break;
        case Mode::Length:          return "LENGTH"; break;
        case Mode::LengthVariation: return "LENGTH VAR"; break;
        case Mode::Note:            return "NOTE"; break;
        case Mode::NoteVariation:   return "NOTE VAR"; break;
        }
        return nullptr;
    }

    void drawDetail(Canvas &canvas, const NoteSequence::Step &step);

    Mode _mode = Mode::Gate;

    StepSelection<CONFIG_STEP_COUNT> _selectedSteps;
};
