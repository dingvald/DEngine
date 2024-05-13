#include "pch.h"
#include "StructureInstance.h"
#include "StructureBaseShape.h"
#include "ProcGen/SpawningAlgorithms/PlaceEntities.h"
#include "ProcGen/SpawningAlgorithms/GenerationContext.h"

drft::StructureInstance::StructureInstance(drft::LayoutPtr layout)
	: _layout(std::move(layout))
{}

void drft::StructureInstance::stamp(sf::Vector2i tileOrigin, const gen::GenerationContext& context, entt::registry& registry) const
{
	for (auto&& [entity, positionList] : *_layout.get())
	{
		gen::placeMany(entity, tileOrigin, positionList, registry);
	}
}
