#include "pch.h"
#include "StructureInstance.h"
#include "ProcGen/PlaceEntities.h"
#include "ProcGen/GenerationContext.h"

using namespace entt::literals;

drft::StructureInstance::StructureInstance(std::unique_ptr<StructureShapeInstance> shape)
	: _shape(std::move(shape))
{
	sf::Vector2i min = { INT_MAX, INT_MAX };
	sf::Vector2i max = { -INT_MAX, -INT_MAX };
	for (auto&& pos : _shape->getPositionsFor("reserved"_hs))
	{
		min.x = std::min(min.x, pos.x);
		min.y = std::min(min.y, pos.y);
		max.x = std::max(max.x, pos.x);
		max.y = std::max(max.y, pos.y);
	}
	_area = sf::IntRect{ min.x, min.y, max.x - min.x, max.y - min.y };
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

sf::IntRect drft::StructureInstance::getArea() const
{
	return _area;
}
