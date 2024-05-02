#include "pch.h"
#include "StaticStructure.h"
#include "ProcGen/SpawningAlgorithms/ISpawningAlgorithm.h"

sf::IntRect drft::StaticStructure::stamp(sf::Vector2i origin, entt::registry& registry) const
{
    for (auto& [entity, positions] : _entities)
    {
        gen::place(entity, origin, positions, registry);
    }
    return {origin.x, origin.y, getMaximumBounds().x, getMaximumBounds().y};
}

void drft::StaticStructure::addEntity(const std::string& entity, std::vector<sf::Vector2i> positions)
{
    _entities.emplace(entity, std::move(positions));
}

void drft::StaticStructure::setCanMirror(bool val)
{
    _canMirror = val;
}

void drft::StaticStructure::setCanRotate(bool val)
{
    _canRotate = val;
}
