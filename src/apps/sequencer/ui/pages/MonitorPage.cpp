#include "MonitorPage.h"

#include "ui/painters/WindowPainter.h"

#include "engine/CVInput.h"
#include "engine/CVOutput.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    CvIn    = 0,
    CvOut   = 1,
    Exit    = 4,
};

static const char *functionNames[] = { "CV IN", "CV OUT", nullptr, nullptr, "EXIT" };


MonitorPage::MonitorPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void MonitorPage::enter() {
}

void MonitorPage::exit() {
}

void MonitorPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "MONITOR");
    WindowPainter::drawActiveFunction(canvas, functionNames[int(_mode)]);
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setFont(Font::Tiny);
    canvas.setColor(0xf);

    switch (_mode) {
    case Mode::CvIn: drawCvIn(canvas); break;
    case Mode::CvOut: drawCvOut(canvas); break;
    }
}

void MonitorPage::updateLeds(Leds &leds) {
}

void MonitorPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::CvIn:
            _mode = Mode::CvIn;
            break;
        case Function::CvOut:
            _mode = Mode::CvOut;
            break;
        case Function::Exit:
            close();
            break;
        }
    }
}

void MonitorPage::encoder(EncoderEvent &event) {
}

void MonitorPage::drawCvIn(Canvas &canvas) {
    FixedStringBuilder<16> string;

    int w = Width / CVInput::Channels;

    for (size_t i = 0; i < CVInput::Channels; ++i) {
        string.reset();
        string("CV%d", i + 1);
        canvas.drawTextCentered(i * w, 32 - 8, w, 8, string);

        string.reset();
        string("%.2fV", _engine.cvInput().channel(i));
        canvas.drawTextCentered(i * w, 32, w, 8, string);
    }
}

void MonitorPage::drawCvOut(Canvas &canvas) {
    FixedStringBuilder<16> string;

    int w = Width / CVOutput::Channels;

    for (size_t i = 0; i < CVOutput::Channels; ++i) {
        string.reset();
        string("CV%d", i + 1);
        canvas.drawTextCentered(i * w, 32 - 8, w, 8, string);

        string.reset();
        string("%.2fV", _engine.cvOutput().channel(i));
        canvas.drawTextCentered(i * w, 32, w, 8, string);
    }
}
