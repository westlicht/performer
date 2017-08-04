#pragma once

#include "Keys.h"

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
    EncoderEvent(Type type, int value) :
        Event(type),
        _value(value)
    {}

    int value() const { return _value; }

private:
    int _value;
};
