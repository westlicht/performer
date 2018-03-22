#include "MonitorPage.h"

#include "ui/painters/WindowPainter.h"

#include "engine/CvInput.h"
#include "engine/CvOutput.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    CvIn    = 0,
    CvOut   = 1,
};

static const char *functionNames[] = { "CV IN", "CV OUT", nullptr, nullptr, nullptr };


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

    if (key.pageModifier()) {
        return;
    }

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::CvIn:
            _mode = Mode::CvIn;
            break;
        case Function::CvOut:
            _mode = Mode::CvOut;
            break;
        }
    }
}

void MonitorPage::encoder(EncoderEvent &event) {
}

void MonitorPage::drawCvIn(Canvas &canvas) {
    FixedStringBuilder<16> str;

    int w = Width / 4;
    int h = 8;

    for (size_t i = 0; i < CvInput::Channels; ++i) {
        int x = i * w;
        int y = 32;

        str.reset();
        str("CV%d", i + 1);
        canvas.drawTextCentered(x, y - h, w, h, str);

        str.reset();
        str("%.2fV", _engine.cvInput().channel(i));
        canvas.drawTextCentered(x, y, w, h, str);
    }
}

void MonitorPage::drawCvOut(Canvas &canvas) {
    FixedStringBuilder<16> str;

    int w = Width / 4;
    int h = 8;

    for (size_t i = 0; i < CvOutput::Channels; ++i) {
        int x = (i % 4) * w;
        int y = 24 + (i / 4) * 20;

        str.reset();
        str("CV%d", i + 1);
        canvas.drawTextCentered(x, y - h, w, h, str);

        str.reset();
        str("%.2fV", _engine.cvOutput().channel(i));
        canvas.drawTextCentered(x, y, w, h, str);
    }
}
