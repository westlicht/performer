#include "Generator.h"

#include "EuclideanGenerator.h"

#include "core/utils/Container.h"

static Container<EuclideanGenerator> generatorContainer;

Generator *Generator::create(Generator::Mode mode, SequenceBuilder &builder) {
    switch (mode) {
    case Mode::Euclidean:
        return generatorContainer.create<EuclideanGenerator>(builder);
    case Mode::Last:
        break;
    }

    return nullptr;
}
