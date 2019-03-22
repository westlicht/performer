#pragma once

#include "ListPage.h"

#include "ui/model/OutputListModel.h"

#include "model/MidiOutput.h"

#include "engine/MidiLearn.h"

class MidiOutputPage : public ListPage {
public:
    MidiOutputPage(PageManager &manager, PageContext &context);

    void reset();

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;

    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void showOutput(int outputIndex);
    void selectOutput(int outputIndex);

    OutputListModel _outputListModel;
    MidiOutput::Output *_output;
    uint8_t _outputIndex;
    MidiOutput::Output _editOutput;
};
