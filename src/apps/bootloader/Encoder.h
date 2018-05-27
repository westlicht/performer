#pragma once

class Encoder {
public:
    static void init();
    static void deinit();

    static void process();

    static bool down() { return _down; }

    static void reset();
    static bool pressed();
    static bool released();
    static int value();

private:
    static bool _down;
    static bool _pressed;
    static bool _released;
    static int _value;
};
