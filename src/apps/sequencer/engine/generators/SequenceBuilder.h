#pragma once

#include "model/NoteSequence.h"
#include "model/CurveSequence.h"

class SequenceBuilder {
public:
    virtual void clear() = 0;
    virtual void revert() = 0;

    // original sequence
    virtual int length() const = 0;
    virtual float value(int index) const = 0;

    // edit sequence
    virtual void setLength(int length) = 0;
    virtual void setValue(int index, float value) = 0;

    virtual void copyStep(int fromIndex, int toIndex) = 0;
};

template<typename T>
class SequenceBuilderImpl : public SequenceBuilder {
public:
    SequenceBuilderImpl(T &sequence, typename T::Layer layer) :
        _edit(sequence),
        _original(sequence),
        _layer(layer),
        _range(T::layerRange(layer))
    {}

    void clear() override {
        _edit.clear();
    }

    void revert() override {
        _edit = _original;
    }

    int length() const override {
        return _original.lastStep() - _original.firstStep() + 1;
    }

    float value(int index) const override {
        int layerValue = _original.step(_original.firstStep() + index).layerValue(_layer);
        return float(layerValue - _range.min) / (_range.max - _range.min);
    }

    void setLength(int length) override {
        _edit.setFirstStep(0);
        _edit.setLastStep(length - 1);
    }

    void setValue(int index, float value) override {
        int layerValue = std::round(value * (_range.max - _range.min) + _range.min);
        _edit.step(_edit.firstStep() + index).setLayerValue(_layer, layerValue);
    }

    void copyStep(int fromIndex, int toIndex) override {
        _edit.step(_edit.firstStep() + toIndex) = _original.step(_original.firstStep() + fromIndex);
    }

private:
    T &_edit;
    T _original;
    typename T::Layer _layer;
    Types::LayerRange _range;
};

typedef SequenceBuilderImpl<NoteSequence> NoteSequenceBuilder;
typedef SequenceBuilderImpl<CurveSequence> CurveSequenceBuilder;
