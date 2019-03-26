#include "MidiOutputPage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    Prev    = 0,
    Next    = 1,
    Init    = 2,
    Commit  = 4,
};

MidiOutputPage::MidiOutputPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _outputListModel),
    _outputListModel(_editOutput)
{
    showOutput(0);
}

void MidiOutputPage::reset() {
    showOutput(0);
}

void MidiOutputPage::enter() {
    ListPage::enter();
}

void MidiOutputPage::exit() {
    ListPage::exit();
}

void MidiOutputPage::draw(Canvas &canvas) {
    bool showCommit = *_output != _editOutput;
    const char *functionNames[] = { "PREV", "NEXT", "INIT", nullptr, showCommit ? "COMMIT" : nullptr };

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "MIDI OUTPUT");
    WindowPainter::drawActiveFunction(canvas, FixedStringBuilder<16>("OUTPUT %d", _outputIndex + 1));
    WindowPainter::drawFooter(canvas, functionNames, pageKeyState());

    ListPage::draw(canvas);
}

void MidiOutputPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Prev:
            selectOutput(_outputIndex - 1);
            break;
        case Function::Next:
            selectOutput(_outputIndex + 1);
            break;
        case Function::Init:
            _editOutput.clear();
            setSelectedRow(0);
            setEdit(false);
            break;
        case Function::Commit:
            *_output = _editOutput;
            setEdit(false);
            showMessage("OUTPUT CHANGED");
            break;
        }
        event.consume();
    }

    ListPage::keyPress(event);
}

void MidiOutputPage::encoder(EncoderEvent &event) {
    if (!edit() && pageKeyState()[Key::Shift]) {
        selectOutput(_outputIndex + event.value());
        event.consume();
        return;
    }

    ListPage::encoder(event);
}

void MidiOutputPage::showOutput(int outputIndex) {
    _output = &_project.midiOutput().output(outputIndex);
    _outputIndex = outputIndex;
    _editOutput = *_output;

    setSelectedRow(0);
    setEdit(false);
}

void MidiOutputPage::selectOutput(int outputIndex) {
    outputIndex = clamp(outputIndex, 0, CONFIG_MIDI_OUTPUT_COUNT - 1);
    if (outputIndex != _outputIndex) {
        showOutput(outputIndex);
    }
}
