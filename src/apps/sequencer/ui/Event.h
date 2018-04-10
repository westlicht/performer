#pragma once

#include "Key.h"

#include "engine/MidiPort.h"

#include "core/midi/MidiMessage.h"

class Event {
public:
    enum Type {
        KeyUp,
        KeyDown,
        KeyPress,
        Encoder,
        Midi,
    };

    Event(Type type) :
        _type(type)
    {}

    Type type() const { return _type; }

    bool consumed() const { return _consumed; }
    void consume() { _consumed = true; }

    template<typename T>
    inline T &as() {
        return *static_cast<T *>(this);
    }

private:
    Type _type;
    bool _consumed = false;
};

class KeyEvent : public Event {
public:
    KeyEvent(Type type, const Key &key) :
        Event(type),
        _key(key)
    {}

    const Key &key() const { return _key; }

private:
    Key _key;
};

class KeyPressEvent : public KeyEvent {
public:
    KeyPressEvent(Type type, const Key &key, int count) :
        KeyEvent(type, key),
        _count(count)
    {}

    int count() const { return _count; }

private:
    int _count;
};

class EncoderEvent : public Event {
public:
    EncoderEvent(int value, bool pressed) :
        Event(Event::Encoder),
        _value(value),
        _pressed(pressed)
    {}

    int value() const { return _value; }
    bool pressed() const { return _pressed; }

private:
    int _value;
    bool _pressed;
};

class MidiEvent : public Event {
public:
    MidiEvent(MidiPort port, const MidiMessage &message) :
        Event(Event::Midi),
        _port(port),
        _message(message)
    {}

    MidiPort port() const { return _port; }

    const MidiMessage &message() const { return _message; }

private:
    MidiPort _port;
    MidiMessage _message;
};
