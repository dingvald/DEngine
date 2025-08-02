#pragma once
#include <ProcGen/GenerationRegistries.h>
#include <ProcGen/Decorators/DecoratorFactory.h>
#include <ProcGen/PositionSelector/PositionSelectorFactory.h>

struct GenerationFinalizationContext
{
	const GenerationRegistries& generationRegistries;
	const DecoratorFactory& decoratorFactory;
	const PositionSelectorFactory& positionSelectorFactory;
};