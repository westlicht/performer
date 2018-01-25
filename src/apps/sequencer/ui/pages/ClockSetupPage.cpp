#include "ClockSetupPage.h"

#include "core/utils/StringBuilder.h"

static MenuPage::Options options {
    .title = "CLOCK SETUP",
    .itemCount = ClockSetupPage::Last,
};

ClockSetupPage::ClockSetupPage(PageManager &manager, PageContext &context) :
    MenuPage(manager, context, options)
{
}

void ClockSetupPage::draw(Canvas &canvas) {
    MenuPage::draw(canvas);

    const auto &clockSetup = _project.clockSetup();

    drawItemName(canvas, itemName(Item(_selectedItem)));

    switch (_selectedItem) {
    case Mode:
        drawItemValue(canvas, ClockSetup::modeName(clockSetup.mode()));
        break;
    case ClockInputPPQN:
        drawItemValue(canvas, clockSetup.clockInputPPQN());
        break;
    case ClockInputMode:
        drawItemValue(canvas, ClockSetup::clockModeName(clockSetup.clockInputMode()));
        break;
    case ClockOutputPPQN:
        drawItemValue(canvas, clockSetup.clockOutputPPQN());
        break;
    case ClockOutputPulse:
        drawItemValue(canvas, clockSetup.clockOutputPulse(), "ms");
        break;
    case ClockOutputMode:
        drawItemValue(canvas, ClockSetup::clockModeName(clockSetup.clockOutputMode()));
        break;
    case MidiRx:
        drawItemValue(canvas, clockSetup.midiRx());
        break;
    case MidiTx:
        drawItemValue(canvas, clockSetup.midiTx());
        break;
    case UsbRx:
        drawItemValue(canvas, clockSetup.usbRx());
        break;
    case UsbTx:
        drawItemValue(canvas, clockSetup.usbTx());
        break;
    }
}

// TODO move those
int adjusted(int value, int offset, int min, int max) {
    return std::max(min, std::min(max, value + offset));
}

template<typename Enum>
Enum adjustedEnum(Enum value, int offset) {
    return Enum(adjusted(int(value), offset, 0, int(Enum::Last) - 1));
}

void ClockSetupPage::encoder(EncoderEvent &event) {
    auto &clockSetup = _project.clockSetup();

    switch (_selectedItem) {
    case Mode:
        clockSetup.setMode(adjustedEnum(clockSetup.mode(), event.value()));
        break;
    case ClockInputPPQN:
        clockSetup.setClockInputPPQN(adjusted(clockSetup.clockInputPPQN(), event.value(), 1, 128));
        break;
    case ClockInputMode:
        clockSetup.setClockInputMode(adjustedEnum(clockSetup.clockInputMode(), event.value()));
        break;
    case ClockOutputPPQN:
        clockSetup.setClockOutputPPQN(adjusted(clockSetup.clockOutputPPQN(), event.value(), 1, 128));
        break;
    case ClockOutputPulse:
        clockSetup.setClockOutputPulse(adjusted(clockSetup.clockOutputPulse(), event.value(), 1, 5));
        break;
    case ClockOutputMode:
        clockSetup.setClockOutputMode(adjustedEnum(clockSetup.clockOutputMode(), event.value()));
        break;
    case MidiRx:
        clockSetup.setMidiRx(event.value() > 0);
        break;
    case MidiTx:
        clockSetup.setMidiTx(event.value() > 0);
        break;
    case UsbRx:
        clockSetup.setUsbRx(event.value() > 0);
        break;
    case UsbTx:
        clockSetup.setUsbTx(event.value() > 0);
        break;
    }

    MenuPage::encoder(event);
}
