#include <pch.h>
#include "IFeature.h"
#include <ProcGen/GenerationContext.h>
#include <Spatial/Helpers.h>

namespace Internal 
{
	static const sf::IntRect InfiniteRect = {
		sf::Vector2i{std::numeric_limits<int>::max(), std::numeric_limits<int>::max()},
		sf::Vector2i{std::numeric_limits<int>::min(), std::numeric_limits<int>::min()},
	};

	static sf::IntRect fitArea(const SlotPositionList& positions)
	{
		sf::IntRect result = InfiniteRect;
		for (auto&& [slot, position, priority] : positions)
		{
			result = drft::spatial::expandToFit(result, drft::spatial::toXY(position));
		}
		return result;
	}
}

GeneratedFeature IFeature::generate(sf::Vector3i position, const GenerationContext& context) const
{
	auto taggedPositions = this->generateTags(position, context);
	auto result = decorate(taggedPositions, context);
	result.feature = this;
	result.area = Internal::fitArea(result.slotPositions);

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