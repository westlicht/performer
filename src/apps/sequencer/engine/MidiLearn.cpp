#include "MidiLearn.h"

MidiLearn::MidiLearn() {
    reset();
}

void MidiLearn::start(ResultCallback callback) {
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
            _messageCounters[int(Controller::Note)] = 0;
            _note = message.note();
        }
        ++_messageCounters[int(Controller::Note)];
    } else if (message.isPitchBend()) {
        ++_messageCounters[int(Controller::PitchBend)];
    } else if (message.isControlChange()) {
        if (message.controllerNumber() != _controllerNumber) {
            _messageCounters[int(Controller::AbsoluteCC)] = 0;
            _messageCounters[int(Controller::RelativeCC)] = 0;
            _controllerNumber = message.controllerNumber();
        }
        int value = message.controllerValue();
        if ((value > 0 && value < 8) || (value > 64 && value < (64 + 8))) {
            ++_messageCounters[int(Controller::RelativeCC)];
        } else {
            ++_messageCounters[int(Controller::AbsoluteCC)];
        }
    } else {
        return;
    }

    if (_messageCounters[int(Controller::Note)] >= 4) {
        emitResult(Result({_port, uint8_t(_channel), Controller::Note, { uint8_t(_note) }}));
    } else if (_messageCounters[int(Controller::PitchBend)] >= 8) {
        emitResult(Result({_port, uint8_t(_channel), Controller::PitchBend, { 0 }}));
    } else if (_messageCounters[int(Controller::AbsoluteCC)] >= 8) {
        emitResult(Result({_port, uint8_t(_channel), Controller::AbsoluteCC, { uint8_t(_controllerNumber)}}));
    } else if (_messageCounters[int(Controller::RelativeCC)] >= 8) {
        emitResult(Result({_port, uint8_t(_channel), Controller::RelativeCC, { uint8_t(_controllerNumber)}}));
    }
}

void MidiLearn::reset() {
    _port = MidiPort(-1);
    _channel = -1;
    _controllerNumber = -1;
    _note = -1;
    _messageCounters.fill(0);
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
