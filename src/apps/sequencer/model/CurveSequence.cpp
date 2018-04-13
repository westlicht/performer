#include "CurveSequence.h"

#include "ModelUtils.h"

Types::LayerRange CurveSequence::layerRange(Layer layer) {
    #define CASE(_name_) \
    case Layer::_name_: \
        return { _name_::Min, _name_::Max };

    switch (layer) {
    case Layer::Shape: return { 0, int(Curve::Last) - 1 };
    CASE(Min)
    CASE(Max)
    }

    #undef CASE

    return { 0, 0 };
}

int CurveSequence::Step::layerValue(Layer layer) const {
    #define CASE(_layer_, _data_) \
    case Layer::_layer_: \
        return _data_;

    switch (layer) {
    CASE(Shape, _shape)
    CASE(Min, _min)
    CASE(Max, _max)
    }

    #undef CASE

    return 0;
}

void CurveSequence::Step::setLayerValue(Layer layer, int value) {
    #define CASE(_layer_, _data_) \
    case Layer::_layer_: \
        _data_ = value; \
        break;

    switch (layer) {
    CASE(Shape, _shape)
    CASE(Min, _min)
    CASE(Max, _max)
    }

    #undef CASE
}

void CurveSequence::Step::clear() {
    setShape(0);
    setMin(0);
    setMax(Max::Max);
}

void CurveSequence::Step::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_shape);
    writer.write(_min);
    writer.write(_max);
}

void CurveSequence::Step::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_shape);
    reader.read(_min);
    reader.read(_max);
}

void CurveSequence::clear() {
    setRange(Types::VoltageRange::Bipolar5V);
    setDivisor(12);
    setResetMeasure(0);
    setRunMode(Types::RunMode::Forward);
    setFirstStep(0);
    setLastStep(15);
    clearSteps();
}

void CurveSequence::clearSteps() {
    for (auto &step : _steps) {
        step.clear();
    }
}

void CurveSequence::setShapes(std::initializer_list<int> shapes) {
    size_t step = 0;
    for (auto shape : shapes) {
        if (step < _steps.size()) {
            _steps[step++].setShape(shape);
        }
    }
}

void CurveSequence::shiftSteps(int direction) {
    ModelUtils::shiftSteps(_steps, direction);
}

void CurveSequence::duplicateSteps() {
    ModelUtils::duplicateSteps(_steps, firstStep(), lastStep());
    setLastStep(lastStep() + (lastStep() - firstStep() + 1));
}

void CurveSequence::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_range);
    writer.write(_divisor);
    writer.write(_resetMeasure);
    writer.write(_runMode);
    writer.write(_firstStep);
    writer.write(_lastStep);

    writeArray(context, _steps);
}

void CurveSequence::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_range);
    reader.read(_divisor);
    reader.read(_resetMeasure);
    reader.read(_runMode);
    reader.read(_firstStep);
    reader.read(_lastStep);

    readArray(context, _steps);
}
