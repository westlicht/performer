#include "GeneratorPage.h"

#include "ui/painters/WindowPainter.h"

#include "engine/generators/Generator.h"
#include "engine/generators/EuclideanGenerator.h"

enum class ContextAction {
    Commit,
    Revert,
    Last
};

const ContextMenuModel::Item contextMenuItems[] = {
    { "COMMIT" },
    { "REVERT" },
};

GeneratorPage::GeneratorPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void GeneratorPage::show(Generator *generator) {
    _generator = generator;

    BasePage::show();
}

void GeneratorPage::enter() {
}

void GeneratorPage::exit() {
}

void GeneratorPage::draw(Canvas &canvas) {
    const char *functionNames[5];
    for (int i = 0; i < 5; ++i) {
        functionNames[i] = i < _generator->paramCount() ? _generator->paramName(i) : nullptr;
    }

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "GENERATOR");
    WindowPainter::drawActiveFunction(canvas, _generator->name());
    WindowPainter::drawFooter(canvas, functionNames, _keyState);

    canvas.setFont(Font::Small);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);

    auto drawValue = [&] (int index, const char *str) {
        int w = Width / 5;
        int x = (Width * index) / 5;
        int y = Height - 16;
        canvas.drawText(x + (w - canvas.textWidth(str)) / 2, y, str);
    };

    for (int i = 0; i < _generator->paramCount(); ++i) {
        FixedStringBuilder<8> str;
        _generator->printParam(i, str);
        drawValue(i, str);
    }

    switch (_generator->mode()) {
    case Generator::Mode::Euclidean:
        drawEuclideanGenerator(canvas, *static_cast<const EuclideanGenerator *>(_generator));
        break;
    case Generator::Mode::Last:
        break;
    }
}

void GeneratorPage::updateLeds(Leds &leds) {
}

void GeneratorPage::keyDown(KeyEvent &event) {
    if (event.key().isGlobal()) {
        return;
    }

    event.consume();
}

void GeneratorPage::keyUp(KeyEvent &event) {
    if (event.key().isGlobal()) {
        return;
    }

    event.consume();
}

void GeneratorPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        contextShow();
        event.consume();
        return;
    }

    if (key.isGlobal()) {
        return;
    }

    event.consume();
}

void GeneratorPage::encoder(EncoderEvent &event) {
    bool changed = false;

    for (int i = 0; i < _generator->paramCount(); ++i) {
        if (_keyState[Key::F0 + i]) {
            _generator->editParam(i, event.value(), event.pressed());
            changed = true;
        }
    }

    if (changed) {
        _generator->update();
    }
}

void GeneratorPage::drawEuclideanGenerator(Canvas &canvas, const EuclideanGenerator &generator) const {
    auto steps = generator.steps();
    const auto &pattern = generator.pattern();

    int stepWidth = Width / steps;
    int stepHeight = 8;
    int x = (Width - steps * stepWidth) / 2;
    int y = Height / 2 - stepHeight / 2;

    for (int i = 0; i < steps; ++i) {
        canvas.setColor(0x7);
        canvas.drawRect(x + 1, y + 1, stepWidth - 2, stepHeight - 2);
        if (pattern[i]) {
            canvas.setColor(0xf);
            canvas.fillRect(x + 1, y + 1, stepWidth - 2, stepHeight - 2);
        }
        x += stepWidth;
    }
}

void GeneratorPage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void GeneratorPage::contextAction(int index) {
    switch (ContextAction(index)) {
    case ContextAction::Commit:
        commit();
        break;
    case ContextAction::Revert:
        revert();
        break;
    case ContextAction::Last:
        break;
    }
}

bool GeneratorPage::contextActionEnabled(int index) const {
    return true;
}

void GeneratorPage::commit() {
    close();
}

void GeneratorPage::revert() {
    _generator->revert();
    close();
}
