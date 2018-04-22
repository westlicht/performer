#include "CalibrationPage.h"

#include "ui/pages/Pages.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

enum class ContextAction {
    Init,
    Load,
    Save,
    Fill,
    Last
};

const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "LOAD" },
    { "SAVE" },
    { "FILL" },
};

CalibrationPage::CalibrationPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _cvOutputListModel),
    _calibration(context.model.settings().calibration())
{
    setOutputIndex(_project.selectedTrackIndex());
}

void CalibrationPage::enter() {
    DBG("enter");
    _engine.setGateOutput(0xff);
    _engine.setGateOutputOverride(true);
    _engine.setCvOutputOverride(true);

    updateOutputs();
}

void CalibrationPage::exit() {
    DBG("exit");
    _engine.setGateOutputOverride(false);
    _engine.setCvOutputOverride(false);
}

void CalibrationPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "CALIBRATION");
    FixedStringBuilder<8> str("CV%d", _outputIndex + 1);
    WindowPainter::drawActiveFunction(canvas, str);
    WindowPainter::drawFooter(canvas);

    ListPage::draw(canvas);
}

void CalibrationPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        contextShow();
        event.consume();
        return;
    }

    if (key.isTrack()) {
        setOutputIndex(key.track());
    }

    ListPage::keyPress(event);

    updateOutputs();
}

void CalibrationPage::encoder(EncoderEvent &event) {
    ListPage::encoder(event);

    updateOutputs();
}

void CalibrationPage::setOutputIndex(int index) {
    _outputIndex = index;
    _cvOutputListModel.setCvOutput(_calibration.cvOutput(index));
}

void CalibrationPage::updateOutputs() {
    float volts = Calibration::CvOutput::itemToVolts(selectedRow());
    for (int i = 0; i < CONFIG_CV_OUTPUT_CHANNELS; ++i) {
        _engine.setCvOutput(i, volts);
    }
}

void CalibrationPage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void CalibrationPage::contextAction(int index) {
    switch (ContextAction(index)) {
    case ContextAction::Init:
        initCalibration();
        break;
    case ContextAction::Load:
        loadCalibration();
        break;
    case ContextAction::Save:
        saveCalibration();
        break;
    case ContextAction::Fill:
        fillCalibration();
        break;
    case ContextAction::Last:
        break;
    }
}

bool CalibrationPage::contextActionEnabled(int index) const {
    return true;
}

void CalibrationPage::initCalibration() {
    _calibration.clear();
    showMessage("CALIBRATION INITIALIZED");
}

void CalibrationPage::loadCalibration() {
}

void CalibrationPage::saveCalibration() {
}

void CalibrationPage::fillCalibration() {
    _calibration.cvOutput(_outputIndex).autoFill();
}
