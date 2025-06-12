#include <pch.h>
#include "IFeature.h"
#include <ProcGen/GenerationContext.h>

GeneratedFeature IFeature::generate(sf::Vector3i position, const GenerationContext& context) const
{
	auto taggedPositions = this->generateTags(position, context);
	auto result = decorate(taggedPositions, context);
	result.feature = this;

	return result;
}

void IFeature::addDecorator(IDecorator::Ptr&& decorator)
{
	_decorators.emplace_back(std::move(decorator));
}

GeneratedFeature IFeature::decorate(TaggedPositions& taggedPositions, const GenerationContext& context) const
{
	GeneratedFeature result;
	for (auto&& decorator : _decorators)
	{
		decorator->decorate(result.slotPositions, taggedPositions, context);
	}
	
	return result;
}
