#include "Generator.h"

#include "EuclideanGenerator.h"
#include "RandomGenerator.h"

#include "core/utils/Container.h"

static Container<EuclideanGenerator, RandomGenerator> generatorContainer;

static void initLayer(SequenceBuilder &builder) {
    builder.clearLayer();
}

Generator *Generator::execute(Generator::Mode mode, SequenceBuilder &builder) {
    switch (mode) {
    case Mode::InitLayer:
        initLayer(builder);
        return nullptr;
    case Mode::Euclidean:
        return generatorContainer.create<EuclideanGenerator>(builder);
    case Mode::Random:
        return generatorContainer.create<RandomGenerator>(builder);
    case Mode::Last:
        break;
    }

    return nullptr;
}
