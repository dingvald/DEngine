#include "pch.h"
#include "StructureInstance.h"

drft::StructureInstance::StructureInstance(std::unordered_map<entt::entity, std::vector<sf::Vector2i>> layout)
	: _layout(layout)
{}

void drft::StructureInstance::stamp(sf::Vector2i tileOrigin, entt::registry& registry) const
{
}
