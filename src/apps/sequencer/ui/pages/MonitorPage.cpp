#include "MonitorPage.h"

#include "ui/painters/WindowPainter.h"

#include "engine/CvInput.h"
#include "engine/CvOutput.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    CvIn    = 0,
    CvOut   = 1,
    Midi    = 2,
    Stats   = 3,
};

static const char *functionNames[] = { "CV IN", "CV OUT", "MIDI", "STATS", nullptr };

static void formatMidiPort(StringBuilder &str, MidiPort port) {
    switch (port) {
    case MidiPort::Midi:
        str("MIDI");
        break;
    case MidiPort::UsbMidi:
        str("USB MIDI");
        break;
    }
}

static void formatMidiMessage(StringBuilder &str1, StringBuilder &str2, const MidiMessage &msg) {
    if (msg.isChannelMessage()) {
        switch (msg.channelMessage()) {
        case MidiMessage::NoteOff:
            str1("NOTE OFF");
            str2("CH=%d NOTE=%d VEL=%d", msg.channel(), msg.note(), msg.velocity());
            return;
        case MidiMessage::NoteOn:
            str1("NOTE ON");
            str2("CH=%d NOTE=%d VEL=%d", msg.channel(), msg.note(), msg.velocity());
            return;
        case MidiMessage::KeyPressure:
            str1("KEY PRESSURE");
            str2("CH=%d NOTE=%d PRE=%d", msg.channel(), msg.note(), msg.keyPressure());
            return;
        case MidiMessage::ControlChange:
            str1("CONTROL CHANGE");
            str2("CH=%d NUM=%d VAL=%d", msg.channel(), msg.controlNumber(), msg.controlValue());
            return;
        case MidiMessage::ProgramChange:
            str1("PROGRAM CHANGE");
            str2("CH=%d NUM=%d", msg.channel(), msg.programNumber());
            return;
        case MidiMessage::ChannelPressure:
            str1("CHANNEL PRESSURE");
            str2("CH=%d PRE=%d", msg.channel(), msg.channelPressure());
            return;
        case MidiMessage::PitchBend:
            str1("PITCH BEND");
            str2("CH=%d VAL=%d", msg.channel(), msg.pitchBend());
            return;
        }
    } else if (msg.isSystemMessage()) {
        switch (msg.systemMessage()) {
        case MidiMessage::SystemExclusive:
            str1("SYSEX");
            return;
        case MidiMessage::TimeCode:
            str1("TIME CODE");
            str2("DATA=%02x", msg.data0());
            return;
        case MidiMessage::SongPosition:
            str1("SONG POSITION");
            str2("POS=%d", msg.songPosition());
            return;
        case MidiMessage::SongSelect:
            str1("SONG SELECT");
            str2("NUM=%d", msg.songNumber());
            return;
        case MidiMessage::TuneRequest:
            str1("TUNE REQUEST");
            return;
        default: break;
        }
    }
}

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
    WindowPainter::drawFooter(canvas, functionNames, pageKeyState(), int(_mode));

    canvas.setBlendMode(BlendMode::Set);
    canvas.setFont(Font::Tiny);
    canvas.setColor(0xf);

    switch (_mode) {
    case Mode::CvIn:
        drawCvIn(canvas);
        break;
    case Mode::CvOut:
        drawCvOut(canvas);
        break;
    case Mode::Midi:
        drawMidi(canvas);
        break;
    case Mode::Stats:
        drawStats(canvas);
        break;
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
        case Function::Midi:
            _mode = Mode::Midi;
            break;
        case Function::Stats:
            _mode = Mode::Stats;
            break;
        }
    }
}

void MonitorPage::encoder(EncoderEvent &event) {
}

void MonitorPage::midi(MidiEvent &event) {
    _lastMidiMessage = event.message();
    _lastMidiMessagePort = event.port();
    _lastMidiMessageTicks = os::ticks();
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
        int y = 20 + (i / 4) * 20;

        str.reset();
        str("CV%d", i + 1);
        canvas.drawTextCentered(x, y - h, w, h, str);

        str.reset();
        str("%.2fV", _engine.cvOutput().channel(i));
        canvas.drawTextCentered(x, y, w, h, str);
    }
}

void MonitorPage::drawMidi(Canvas &canvas) {

    if (os::ticks() - _lastMidiMessageTicks < os::time::ms(1000)) {
        FixedStringBuilder<32> str1;

        formatMidiPort(str1, _lastMidiMessagePort);
        canvas.drawTextCentered(0, 24 - 8, Width, 16, str1);

        str1.reset();
        FixedStringBuilder<32> str2;

        formatMidiMessage(str1, str2, _lastMidiMessage);
        canvas.drawTextCentered(0, 32 - 8, Width, 16, str1);
        canvas.drawTextCentered(0, 40 - 8, Width, 16, str2);
    }
}

void MonitorPage::drawStats(Canvas &canvas) {
    auto stats = _engine.stats();

    auto drawValue = [&] (int index, const char *name, const char *value) {
        canvas.drawText(10, 20 + index * 10, name);
        canvas.drawText(100, 20 + index * 10, value);
    };

    {
        int seconds = stats.uptime;
        int minutes = seconds / 60;
        int hours = minutes / 60;
        FixedStringBuilder<16> str("%d:%02d:%02d", hours, minutes % 60, seconds % 60);
        drawValue(0, "UPTIME:", str);
    }

    {
        FixedStringBuilder<16> str("%d", stats.midiRxOverflow);
        drawValue(1, "MIDI OVF:", str);
    }

    {
        FixedStringBuilder<16> str("%d", stats.usbMidiRxOverflow);
        drawValue(2, "USBMIDI OVF:", str);
    }

}
