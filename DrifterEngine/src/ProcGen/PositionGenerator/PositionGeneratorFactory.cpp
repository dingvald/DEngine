#include "pch.h"
#include "PositionGeneratorFactory.h"
#include "PositionGeneratorIds.h"

#include <ProcGen/PositionGenerator/HardcodedPositionGenerator.h>

void bindPositionGenerators(PositionGeneratorFactory& generatorFactory)
{
    // Example registration:
    // generatorFactory.registerType<RandomPositionGenerator>(PositionGeneratorId::Random);
    // Add more registrations as needed
    generatorFactory.registerType<HardcodedPositionGenerator>(PositionGeneratorId::Hardcoded);
}
