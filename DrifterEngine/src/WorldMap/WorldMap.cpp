#include "pch.h"
#include "WorldMap.h"

void drft::WorldMap::init(sf::Vector2i dimensions, unsigned int seed)
{
	_dimensions = dimensions;

	_worldGenerator.init(dimensions, seed);
	_worldGenerator.loadBiomes("biomes.json");
	_worldGenerator.generateTerrain();
}

sf::Vector2i drft::WorldMap::getDimensions() const
{
	return _dimensions;
}

drft::gen::BiomeIcon drft::WorldMap::getBiomeIcon(sf::Vector2i coordinate) const
{
	return _worldGenerator.getBiomeIcon(coordinate);
}

void drft::WorldMap::finalizeBuild(sf::Vector2i coordinate, entt::registry& registry) const
{
	_worldGenerator.finalize(coordinate, registry);
}
