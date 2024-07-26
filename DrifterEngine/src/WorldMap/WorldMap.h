#pragma once
#include "Utility/stdHashing.h"
#include "Spatial/WorldMapPosition.h"


class Biome;
namespace drft
{
	namespace gen
	{
		class WorldGenerator;
	}

	class WorldMap
	{
	public:
		WorldMap(const gen::WorldGenerator& generator);

		sf::Vector2i getDimensions() const;
		const Biome* getBiome(WorldMapPosition position) const;

	private:
		const gen::WorldGenerator& _generator;
	};
}


