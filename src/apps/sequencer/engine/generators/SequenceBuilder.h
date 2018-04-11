#pragma once

class SequenceBuilder {
public:
    virtual void clear() = 0;
    virtual void revert() = 0;

    // original sequence
    virtual int length() const = 0;
    virtual bool gate(int index) const = 0;
    virtual float value(int index) const = 0;

    // new sequence
    virtual void setLength(int length) = 0;
    virtual void setGate(int index, bool gate) = 0;
    virtual void setValue(int index, float value) = 0;
};
