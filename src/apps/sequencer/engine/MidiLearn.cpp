#include "MidiLearn.h"

MidiLearn::MidiLearn() {
    reset();
}

void MidiLearn::start(ResultCallback callback) {
    reset();
    _callback = callback;
}

void MidiLearn::stop() {
    _callback = nullptr;
}

void MidiLearn::receiveMidi(MidiPort port, const MidiMessage &message) {
    if (!_callback) {
        return;
    }

    if (port != _port || message.channel() != _channel) {
        reset();
        _port = port;
        _channel = message.channel();
    }

    if (message.isNoteOn() || message.isNoteOff()) {
        if (message.note() != _note) {
            _eventCounters[int(Event::Note)] = 0;
            _note = message.note();
        }
        ++_eventCounters[int(Event::Note)];
    } else if (message.isPitchBend()) {
        ++_eventCounters[int(Event::PitchBend)];
    } else if (message.isControlChange()) {
        // filter NRPN messages (cc number 0-31 followed by 32-63)
        auto controlNumber = message.controlNumber();
        bool isNRPN = _lastControlNumber >= 0 && _lastControlNumber < 32 && controlNumber >= 32 && controlNumber < 64;
        _lastControlNumber = controlNumber;
        if (!isNRPN) {
            if (controlNumber != _controlNumber) {
                _eventCounters[int(Event::ControlAbsolute)] = 0;
                _eventCounters[int(Event::ControlRelative)] = 0;
                _controlNumber = controlNumber;
            }
            int value = message.controlValue();
            if ((value > 0 && value < 8) || (value > 64 && value < (64 + 8))) {
                ++_eventCounters[int(Event::ControlRelative)];
            } else {
                ++_eventCounters[int(Event::ControlAbsolute)];
            }
        }
    } else {
        return;
    }

    if (_eventCounters[int(Event::Note)] >= 2) {
        emitResult(Result({_port, uint8_t(_channel), Event::Note, { uint8_t(_note) }}));
    } else if (_eventCounters[int(Event::PitchBend)] >= 8) {
        emitResult(Result({_port, uint8_t(_channel), Event::PitchBend, { 0 }}));
    } else if (_eventCounters[int(Event::ControlAbsolute)] >= 8) {
        emitResult(Result({_port, uint8_t(_channel), Event::ControlAbsolute, { uint8_t(_controlNumber)}}));
    } else if (_eventCounters[int(Event::ControlRelative)] >= 8) {
        emitResult(Result({_port, uint8_t(_channel), Event::ControlRelative, { uint8_t(_controlNumber)}}));
    }
}

void MidiLearn::reset() {
    _port = MidiPort(-1);
    _channel = -1;
    _controlNumber = -1;
    _lastControlNumber = -1;
    _note = -1;
    _eventCounters.fill(0);
    _lastResult.event = Event::Last;
}

void MidiLearn::emitResult(Result result) {
    reset();

    if (result == _lastResult) {
        return;
    }
    _lastResult = result;

    if (_callback) {
        _callback(result);
    }
}
