#include "pch.h"
#include "StructureInstance.h"
#include "ProcGen/PlaceEntities.h"
#include "ProcGen/GenerationContext.h"

using namespace entt::literals;

drft::StructureInstance::StructureInstance(std::unique_ptr<StructureShapeInstance> shape)
	: _shape(std::move(shape))
{
}

void drft::StructureInstance::stamp(sf::Vector2i tileOrigin, gen::GenerationContext& context) const
{
	for (auto&& [entity, positionList] : _shape->getEntityPositions())
	{
		for (auto&& position : positionList)
		{
			context.entityPositions[entity].insert(tileOrigin + position);
		}
	}
}

const entt::dense_set<sf::Vector2i>& drft::StructureInstance::getArea() const
{
	return _shape->getPositionsFor("reserved"_hs);
}
