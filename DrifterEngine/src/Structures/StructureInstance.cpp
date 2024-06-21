#include "pch.h"
#include "StructureInstance.h"
#include "ProcGen/PlaceEntities.h"
#include "ProcGen/GenerationContext.h"
#include "ProcGen/GridBitFlags.h"

drft::StructureInstance::StructureInstance(std::unique_ptr<StructureShapeInstance> shape)
	: _shape(std::move(shape))
{}

void drft::StructureInstance::stamp(sf::Vector2i tileOrigin, gen::GenerationContext& context) const
{
	for (auto&& pos : _shape->getPositionsFor(StructureBit::Reserved))
	{
		context.grid.at(tileOrigin.x + pos.x, tileOrigin.y + pos.y).set(gen::GridBitFlags::Structure);
	}

	for (auto&& [entity, positionList] : _shape->getEntityPositions())
	{
		gen::placeMany(entity, tileOrigin, positionList, context.registry);
	}
}
