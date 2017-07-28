#include "Console.h"

#include <iostream>

void Console::write(char c) {
    send(c);
}

void Console::write(const char *s) {
    while (*s != '\0') {
        send(*s++);
    }
}

void Console::write(const char *s, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        send(s[i]);
    }
}

void Console::write(const std::string &s) {
    for (const auto c : s) {
        send(char(c));
    }
}

void Console::send(char c) {
    std::cout << c;
}

extern "C" {

int _write(int file, char *data, int len) {
    Console::write(data, len);
    return len;
}

}
