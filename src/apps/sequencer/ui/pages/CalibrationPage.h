#pragma once

#include "ListPage.h"

#include "ui/model/CalibrationCvOutputListModel.h"

#include "model/Calibration.h"

class CalibrationPage : public ListPage {
public:
    CalibrationPage(PageManager &manager, PageContext &context);

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
    void initCalibration();
    void loadCalibration();
    void saveCalibration();
    void fillCalibration();

    void loadCalibrationFromFile();
    void saveCalibrationToFile();

    int _outputIndex;
    Calibration &_calibration;
    CalibrationCvOutputListModel _cvOutputListModel;
};
