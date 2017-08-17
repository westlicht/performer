#pragma once

#include <functional>

class DIO {
public:
    struct Input {
        typedef std::function<void(bool)> Handler;

        bool get() const { return false; }
        void setHandler(Handler handler) { _handler = handler; }

    private:
        Handler _handler;
    };

    struct Output {
        void set(bool value) {}
    };

    void init();

    Input clockInput;
    Input resetInput;

    Output clockOutput;
    Output resetOutput;
};
