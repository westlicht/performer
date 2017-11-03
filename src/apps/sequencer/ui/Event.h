#pragma once

#include "Key.h"

class Event {
public:
    enum Type {
        KeyUp,
        KeyDown,
        Encoder,
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

class EncoderEvent : public Event {
public:
    EncoderEvent(Type type, int value, bool pressed) :
        Event(type),
        _value(value),
        _pressed(pressed)
    {}

    int value() const { return _value; }
    bool pressed() const { return _pressed; }

private:
    int _value;
    bool _pressed;
};
