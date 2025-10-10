#pragma once
#include <Generation/GenerationRegistries.h>
#include <Generation/Decorators/DecoratorFactory.h>
#include <Generation/PositionSelector/PositionSelectorFactory.h>

struct GenerationFinalizationContext
{
	const GenerationRegistries& generationRegistries;
	const DecoratorFactory& decoratorFactory;
	const PositionSelectorFactory& positionSelectorFactory;
};