#include "Generator.h"

#include "EuclideanGenerator.h"
#include "RandomGenerator.h"

#include "core/utils/Container.h"

static Container<EuclideanGenerator, RandomGenerator> generatorContainer;
static EuclideanGenerator::Params euclideanParams;
static RandomGenerator::Params randomParams;

static void initLayer(SequenceBuilder &builder) {
    builder.clearLayer();
}

Generator *Generator::execute(Generator::Mode mode, SequenceBuilder &builder) {
    switch (mode) {
    case Mode::InitLayer:
        initLayer(builder);
        return nullptr;
    case Mode::Euclidean:
        return generatorContainer.create<EuclideanGenerator>(builder, euclideanParams);
    case Mode::Random:
        return generatorContainer.create<RandomGenerator>(builder, randomParams);
    case Mode::Last:
        break;
    }

    return nullptr;
}
