#pragma once
#include <Factory/Factory.h>
#include <ProcGen/PositionGenerator/IPositionGenerator.h>

using PositionGeneratorFactory = Factory<IPositionGenerator>;

void bindPositionGenerators(PositionGeneratorFactory& generatorFactory);
