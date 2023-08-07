#include "pch.h"
#include "WorldMap.h"

void drft::WorldMap::init(sf::Vector2i dimensions, unsigned int seed)
{
	_dimensions = dimensions;
	_worldGenerator.setSeed(seed);


}

sf::Vector2i drft::WorldMap::getDimensions() const
{
	return _dimensions;
}

void drft::WorldMap::generate(sf::Vector2i coordinate, entt::registry& registry) const
{
	_worldGenerator.generate(coordinate, registry);
}
