#pragma once

#include "SequenceBuilder.h"

#include "core/utils/StringBuilder.h"

class Generator {
public:
    enum class Mode {
        Euclidean,
        Last
    };

    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::Euclidean:   return "Euclidean";
        case Mode::Last:        break;
        }
        return nullptr;
    }

    Generator(SequenceBuilder &builder) :
        _builder(builder)
    {}

    virtual Mode mode() const = 0;
    const char *name() const { return modeName(mode()); }

    // parameters

    virtual int paramCount() const = 0;
    virtual const char *paramName(int index) const = 0;
    virtual void editParam(int index, int value, bool shift) = 0;
    virtual void printParam(int index, StringBuilder &str) const = 0;


    virtual void revert() {
        _builder.revert();
    }

    virtual void update() = 0;

    static Generator *create(Generator::Mode mode, SequenceBuilder &builder);

protected:
    SequenceBuilder &_builder;
};
