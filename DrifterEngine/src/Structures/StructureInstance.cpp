#include "pch.h"
#include "StructureInstance.h"
#include "ProcGen/PlaceEntities.h"
#include "ProcGen/GenerationContext.h"

using namespace entt::literals;

drft::StructureInstance::StructureInstance(std::unique_ptr<StructureShapeInstance> shape)
	: _shape(std::move(shape))
{}

void drft::StructureInstance::stamp(sf::Vector2i tileOrigin, gen::GenerationContext& context) const
{
	for (auto&& pos : _shape->getPositionsFor(StructureBit::Reserved))
	{
		context.grid.at(tileOrigin.x + pos.x, tileOrigin.y + pos.y).insert("structure"_hs);
	}

	for (auto&& [entity, positionList] : _shape->getEntityPositions())
	{
		for (auto&& position : positionList)
		{
			context.entityPositions[entity].insert(position);
		}
	}
}
