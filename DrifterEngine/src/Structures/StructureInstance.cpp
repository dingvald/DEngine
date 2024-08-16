#include "pch.h"
#include "StructureInstance.h"
#include "ProcGen/PlaceEntities.h"

using namespace entt::literals;

drft::StructureInstance::StructureInstance(std::unique_ptr<StructureShapeInstance> shape)
	: _shape(std::move(shape))
{
}

const entt::dense_set<sf::Vector2i>& drft::StructureInstance::getArea() const
{
	return _shape->getPositionsFor("reserved"_hs);
}
