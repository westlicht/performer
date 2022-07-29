#include "CurveSequence.h"
#include "ProjectVersion.h"
#include "ModelUtils.h"

Types::LayerRange CurveSequence::layerRange(Layer layer) {
    #define CASE(_name_) \
    case Layer::_name_: \
        return { _name_::Min, _name_::Max };

    switch (layer) {
    case Layer::Shape:
    case Layer::ShapeVariation:
        return { 0, int(Curve::Last) - 1 };
    CASE(ShapeVariationProbability)
    CASE(Min)
    CASE(Max)
    CASE(Gate)
    CASE(GateProbability)
    case Layer::Last:
        break;
    }

    #undef CASE

    return { 0, 0 };
}

int CurveSequence::layerDefaultValue(Layer layer)
{
    CurveSequence::Step step;

    switch (layer) {
    case Layer::Shape:
        return step.shape();
    case Layer::ShapeVariation:
        return step.shapeVariation();
    case Layer::ShapeVariationProbability:
        return step.shapeVariationProbability();
    case Layer::Min:
        return step.min();
    case Layer::Max:
        return step.max();
    case Layer::Gate:
        return step.gate();
    case Layer::GateProbability:
        return step.gateProbability();
    case Layer::Last:
        break;
    }

    return 0;
}

int CurveSequence::Step::layerValue(Layer layer) const {
    switch (layer) {
    case Layer::Shape:
        return shape();
    case Layer::ShapeVariation:
        return shapeVariation();
    case Layer::ShapeVariationProbability:
        return shapeVariationProbability();
    case Layer::Min:
        return min();
    case Layer::Max:
        return max();
    case Layer::Gate:
        return gate();
    case Layer::GateProbability:
        return gateProbability();
    case Layer::Last:
        break;
    }

    return 0;
}

void CurveSequence::Step::setLayerValue(Layer layer, int value) {
    switch (layer) {
    case Layer::Shape:
        setShape(value);
        break;
    case Layer::ShapeVariation:
        setShapeVariation(value);
        break;
    case Layer::ShapeVariationProbability:
        setShapeVariationProbability(value);
        break;
    case Layer::Min:
        setMin(value);
        break;
    case Layer::Max:
        setMax(value);
        break;
    case Layer::Gate:
        setGate(value);
        break;
    case Layer::GateProbability:
        setGateProbability(value);
        break;
    case Layer::Last:
        break;
    }
}

void CurveSequence::Step::clear() {
    _data0.raw = 0;
    _data1.raw = 0;
    setShape(0);
    setShapeVariation(0);
    setShapeVariationProbability(0);
    setMin(0);
    setMax(Max::Max);
    setGate(0);
    setGateProbability(GateProbability::Max);
}

void CurveSequence::Step::write(VersionedSerializedWriter &writer) const {
    writer.write(_data0);
    writer.write(_data1);
}

void CurveSequence::Step::read(VersionedSerializedReader &reader) {
    if (reader.dataVersion() < ProjectVersion::Version15) {
        uint8_t shape, min, max;
        reader.read(shape);
        reader.read(min);
        reader.read(max);
        _data0.shape = shape;
        _data0.min = min;
        _data0.max = max;

        if (reader.dataVersion() < ProjectVersion::Version14) {
            if (_data0.shape <= 1) {
                _data0.shape = (_data0.shape + 1) % 2;
            }
        }
    } else {
        reader.read(_data0);
        reader.read(_data1);
    }
}

void CurveSequence::writeRouted(Routing::Target target, int intValue, float floatValue) {
    switch (target) {
    case Routing::Target::Divisor:
        setDivisor(intValue, true);
        break;
    case Routing::Target::RunMode:
        setRunMode(Types::RunMode(intValue), true);
        break;
    case Routing::Target::FirstStep:
        setFirstStep(intValue, true);
        break;
    case Routing::Target::LastStep:
        setLastStep(intValue, true);
        break;
    default:
        break;
    }
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

bool CurveSequence::isEdited() const {
    auto clearStep = Step();
    for (const auto &step : _steps) {
        if (step != clearStep) {
            return true;
        }
    }
    return false;
}

void CurveSequence::setShapes(std::initializer_list<int> shapes) {
    size_t step = 0;
    for (auto shape : shapes) {
        if (step < _steps.size()) {
            _steps[step++].setShape(shape);
        }
    }
}

void CurveSequence::shiftSteps(const std::bitset<CONFIG_STEP_COUNT> &selected, int direction) {
    if (selected.any()) {
        ModelUtils::shiftSteps(_steps, selected, direction);
    } else {
        ModelUtils::shiftSteps(_steps, firstStep(), lastStep(), direction);
    }
}

void CurveSequence::duplicateSteps() {
    ModelUtils::duplicateSteps(_steps, firstStep(), lastStep());
    setLastStep(lastStep() + (lastStep() - firstStep() + 1));
}

void CurveSequence::write(VersionedSerializedWriter &writer) const {
    writer.write(_range);
    writer.write(_divisor.base);
    writer.write(_resetMeasure);
    writer.write(_runMode.base);
    writer.write(_firstStep.base);
    writer.write(_lastStep.base);

    writeArray(writer, _steps);
}

void CurveSequence::read(VersionedSerializedReader &reader) {
    reader.read(_range);
    if (reader.dataVersion() < ProjectVersion::Version10) {
        reader.readAs<uint8_t>(_divisor.base);
    } else {
        reader.read(_divisor.base);
    }
    reader.read(_resetMeasure);
    reader.read(_runMode.base);
    reader.read(_firstStep.base);
    reader.read(_lastStep.base);

    readArray(reader, _steps);
}
