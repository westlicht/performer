#include "ClockSetup.h"
#include "ProjectVersion.h"

void ClockSetup::clear() {
    _mode = Mode::Auto;
    _shiftMode = ShiftMode::Restart;
    _clockInputDivisor = 12;
    _clockInputMode = ClockInputMode::Reset;
    _clockOutputDivisor = 12;
    _clockOutputSwing = false;
    _clockOutputPulse = 1;
    _clockOutputMode = ClockOutputMode::Reset;
    _midiRx = true;
    _midiTx = true;
    _usbRx = false;
    _usbTx = false;
    _dirty = true;
}

void ClockSetup::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_mode);
    writer.write(_shiftMode);
    writer.write(_clockInputDivisor);
    writer.write(_clockInputMode);
    writer.write(_clockOutputDivisor);
    writer.write(_clockOutputSwing);
    writer.write(_clockOutputPulse);
    writer.write(_clockOutputMode);
    writer.write(_midiRx);
    writer.write(_midiTx);
    writer.write(_usbRx);
    writer.write(_usbTx);
}

void ClockSetup::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_mode);
    reader.read(_shiftMode);
    reader.read(_clockInputDivisor);
    reader.read(_clockInputMode);
    reader.read(_clockOutputDivisor);
    reader.read(_clockOutputSwing, ProjectVersion::Version11);
    reader.read(_clockOutputPulse);
    reader.read(_clockOutputMode);
    reader.read(_midiRx);
    reader.read(_midiTx);
    reader.read(_usbRx);
    reader.read(_usbTx);
}
