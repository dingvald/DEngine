#include "pch.h"
#include "StructureInstance.h"
#include "ProcGen/SpawningAlgorithms/PlaceEntities.h"
#include "ProcGen/SpawningAlgorithms/GenerationContext.h"

drft::StructureInstance::StructureInstance(std::unordered_map<std::string, std::vector<sf::Vector2i>> layout)
	: _layout(layout)
{}

void drft::StructureInstance::stamp(sf::Vector2i tileOrigin, const gen::GenerationContext& context, entt::registry& registry) const
{
	for (auto&& [entity, positionList] : _layout)
	{
		gen::placeMany(entity, tileOrigin, positionList, registry);
	}
}
