#pragma once

#include "ListPage.h"

#include "ui/model/CalibrationCvOutputListModel.h"

#include "model/Settings.h"

class SettingsPage : public ListPage {
public:
    SettingsPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;

    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void setOutputIndex(int index);
    void updateOutputs();

    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;

    void initSettings();
    void loadSettings();
    void saveSettings();
    void fillCalibration();

    void loadSettingsFromFile();
    void saveSettingsToFile();

    Settings &_settings;

    int _outputIndex;
    CalibrationCvOutputListModel _cvOutputListModel;
};
