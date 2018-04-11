#include "EuclideanGenerator.h"

EuclideanGenerator::EuclideanGenerator(SequenceBuilder &builder) :
    Generator(builder)
{
    update();
}

int EuclideanGenerator::paramCount() const {
    return int(Param::Last);
}

const char *EuclideanGenerator::paramName(int index) const {
    switch (Param(index)) {
    case Param::Steps:  return "Steps";
    case Param::Beats:  return "Beats";
    case Param::Offset: return "Offset";
    case Param::Last:   break;
    }
    return nullptr;
}

void EuclideanGenerator::editParam(int index, int value, bool shift) {
    switch (Param(index)) {
    case Param::Steps:  setSteps(steps() + value); break;
    case Param::Beats:  setBeats(beats() + value); break;
    case Param::Offset: setOffset(offset() + value); break;
    case Param::Last:   break;
    }
}

void EuclideanGenerator::printParam(int index, StringBuilder &str) const {
    switch (Param(index)) {
    case Param::Steps:  str("%d", _steps); break;
    case Param::Beats:  str("%d", _beats); break;
    case Param::Offset: str("%d", _offset); break;
    case Param::Last:   break;
    }
}

void EuclideanGenerator::update()  {
    _pattern = Rhythm::euclidean(_beats, _steps).shifted(_offset);

    _builder.clear();
    _builder.setLength(_steps);

    for (size_t i = 0; i < _pattern.size(); ++i) {
        _builder.setGate(i, _pattern[i]);
    }
}
