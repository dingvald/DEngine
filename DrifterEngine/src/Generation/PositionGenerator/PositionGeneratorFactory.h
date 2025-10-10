#pragma once
#include <Factory/Factory.h>
#include <Generation/PositionGenerator/IPositionGenerator.h>

using PositionGeneratorFactory = Factory<IPositionGenerator>;

void bindPositionGenerators(PositionGeneratorFactory& generatorFactory);
