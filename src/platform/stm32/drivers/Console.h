#pragma once

#include <string>

class Console {
public:
    static void init();

    static void write(char c);
    static void write(const char *s);
    static void write(const char *s, size_t length);
    static void write(const std::string &s);

private:
    static void send(char c);
};
