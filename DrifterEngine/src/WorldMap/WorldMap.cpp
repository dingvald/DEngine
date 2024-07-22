#include "pch.h"
#include "WorldMap.h"

void drft::WorldMap::create()
{
	_worldGenerator.createWorldMapSettingsFromJSON("world_settings.json");
	_worldGenerator.init();
	_worldGenerator.generate();
}

void drft::WorldMap::load(cereal::JSONInputArchive& iarchive)
{
	_worldGenerator.load(iarchive);
	_worldGenerator.init();
	_worldGenerator.generate();
}

void drft::WorldMap::save(cereal::JSONOutputArchive& oarchive) const
{
	_worldGenerator.save(oarchive);
}

void drft::WorldMap::fixedUpdate(const entt::registry& registry)
{
	_worldGenerator.fixedUpdate(registry);
}

sf::Vector2i drft::WorldMap::getDimensions() const
{
	return _worldGenerator.getDimensions();
}

BiomeIcon drft::WorldMap::getBiomeIcon(sf::Vector2i coordinate) const
{
	return _worldGenerator.getBiomeIcon(coordinate);
}

sf::Vector2i drft::WorldMap::getStartingPosition(const std::string& biomeType) const
{
	return _worldGenerator.getStartingPosition(biomeType);
}

bool drft::WorldMap::generateChunk(sf::Vector2i coordinate, entt::registry& registry) const
{
	const auto status = _worldGenerator.generateChunk(coordinate, registry);
	if (status == drft::gen::GenerationStatus::Done)
	{
		return true;
	}
	return false;
}
