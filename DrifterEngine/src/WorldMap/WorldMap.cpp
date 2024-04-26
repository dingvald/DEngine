#include "pch.h"
#include "WorldMap.h"

void drft::WorldMap::init(sf::Vector2i dimensions, unsigned int seed)
{
	_dimensions = dimensions;

	_worldGenerator.init(dimensions, seed);
	_worldGenerator.generate();
}

sf::Vector2i drft::WorldMap::getDimensions() const
{
	return _dimensions;
}

drft::gen::BiomeIcon drft::WorldMap::getBiomeIcon(sf::Vector2i coordinate) const
{
	return _worldGenerator.getBiomeIcon(coordinate);
}

sf::Vector2i drft::WorldMap::getStartingPosition(const std::string& biomeType) const
{
	return _worldGenerator.getStartingPosition(biomeType);
}

void drft::WorldMap::finalizeBuild(sf::Vector2i coordinate, entt::registry& registry) const
{
	_worldGenerator.finalizeChunk(coordinate, registry);
}
