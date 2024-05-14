#include "pch.h"
#include "StructureInstance.h"
#include "ProcGen/SpawningAlgorithms/PlaceEntities.h"
#include "ProcGen/SpawningAlgorithms/GenerationContext.h"
#include "ProcGen/GridBitFlags.h"

drft::StructureInstance::StructureInstance(std::unique_ptr<StructureShapeInstance> shape)
	: _shape(std::move(shape))
{}

void drft::StructureInstance::stamp(sf::Vector2i tileOrigin, gen::GenerationContext& context, entt::registry& registry) const
{
	for (auto&& pos : _shape->getPositionsFor(StructureBit::Reserved))
	{
		context.grid.at(tileOrigin.x + pos.x, tileOrigin.y + pos.y).set(gen::GridBitFlags::Structure);
	}

	for (auto&& [entity, positionList] : _shape->getEntityPositions())
	{
		gen::placeMany(entity, tileOrigin, positionList, registry);
	}
}
