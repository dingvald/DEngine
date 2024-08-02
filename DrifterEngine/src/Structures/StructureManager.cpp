#include "pch.h"
#include "StructureManager.h"

#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"
#include "ProcGen/WorldGeneration/WorldGenerator.h"
#include "Random/PercentChance.h"

const int SCAN_RADIUS = 20;

using namespace entt::literals;

static const std::filesystem::path STATIC_DATA_PATH = ".\\data\\static\\";
static const std::filesystem::path STRUCTURE_FOLDER_PATH = STATIC_DATA_PATH.string() + "structures";

drft::StructureManager::StructureManager(const gen::WorldGenerator& generator)
	: _generator(generator)
{
	_structureFactory.createStructureBlueprintsFromJSON(STRUCTURE_FOLDER_PATH);
}

void drft::StructureManager::scanForStuctures(sf::Vector2i origin)
{
	auto circle = spatial::getIntCircleInRadius(origin, SCAN_RADIUS);
	for (auto&& chunk : circle)
	{
		if (_visited.contains(chunk)) continue;
		_visited.insert(chunk);

		const Biome* biome = _generator.getBiome(chunk);
		if (!biome) continue;

		for (auto&& [name, probability] : biome->getStructureProbabilities())
		{
			if (!rng::percentChance(probability * 100.0)) continue;
			
			if (auto structure = _structureFactory.build(name))
			{
				_generator.tagArea(spatial::toTileSpace(chunk), structure->getArea(), "structure"_hs);
				_structures.emplace(chunk, std::move(structure));
				break;
			}
		}
	}
}

const drft::StructureInstance* drft::StructureManager::getStructureAt(sf::Vector2i coordinate) const
{
	if (_structures.contains(coordinate))
	{
		return _structures.at(coordinate).get();
	}
	return nullptr;
}
