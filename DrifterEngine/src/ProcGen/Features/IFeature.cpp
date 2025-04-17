#include "pch.h"
#include "IFeature.h"
#include <ProcGen/GenerationContext.h>

FeatureGenerationResult IFeature::generate(sf::Vector2i position, const GenerationContext& context) const
{
	auto taggedPositions = this->doGenerate(context);
	auto result = decorate(taggedPositions, context);
	result.origin = position;
	result.feature = this;

	return std::move(result);
}

void IFeature::addDecorator(IDecorator::Ptr&& decorator)
{
	_decorators.emplace_back(std::move(decorator));
}

FeatureGenerationResult IFeature::decorate(TaggedPositions& taggedPositions, const GenerationContext& context) const
{
	FeatureGenerationResult result;
	for (auto&& decorator : _decorators)
	{
		decorator->decorate(result.slotPositions, taggedPositions, context);
	}
	
	return result;
}
