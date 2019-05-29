#pragma once

#include "BasePage.h"

#include "ui/StepSelection.h"
#include "ui/model/StageSequenceListModel.h"

#include "engine/generators/SequenceBuilder.h"

#include "core/utils/Container.h"

class StageSequenceEditPage : public BasePage {
public:
    StageSequenceEditPage(PageManager &manager, PageContext &context);

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
    typedef StageSequence::Layer Layer;

    static const int StepCount = 16;

    int stepOffset() const { return _section * StepCount; }

    void switchLayer(int functionKey);
    int activeFunctionKey();

    void updateMonitorStep();
    void drawDetail(Canvas &canvas, const StageSequence::Step &step);

    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;

    void initSequence();
    void copySequence();
    void pasteSequence();
    void duplicateSequence();
    void generateSequence();

    void quickEdit(int index);

    bool allSelectedStepsActive() const;
    void setSelectedStepsGate(bool gate);

    StageSequence::Layer layer() const { return _project.selectedStageSequenceLayer(); };
    void setLayer(StageSequence::Layer layer) { _project.setSelectedStageSequenceLayer(layer); }

    int _section = 0;
    bool _showDetail;
    uint32_t _showDetailTicks;

    StageSequenceListModel _listModel;

    StepSelection<CONFIG_STEP_COUNT> _stepSelection;

    // Container<StageSequenceBuilder> _builderContainer;
};
