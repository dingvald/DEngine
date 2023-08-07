#pragma once
#include "PlacementAlgorithms/GridEnums.h"
#include "Random/PerlinNoise.h"
#include "Factory/MachineFactory.h"

namespace drft::spatial
{
	template<typename T>
	class Grid;
}

namespace drft::gen
{
	class WorldGenerator
	{
	public:
		WorldGenerator();
		void setSeed(unsigned int seed);
		void generate(sf::Vector2i coordinate, entt::registry& registry) const;


	private:
		void registerMachines();

	private:
		unsigned int _seed = 0;

		using NoisePtr = std::unique_ptr<rng::PerlinNoise>;
		NoisePtr _temperatureNoise;
		NoisePtr _altitudeNoise;
		NoisePtr _moistureNoise;
	};
}


