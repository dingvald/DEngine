#pragma once
#include "ProcGen/WorldGenerator.h"
#include "Utility/stdHashing.h"

namespace drft
{
	class WorldMap
	{
	public:
		void init(sf::Vector2i dimensions, unsigned int seed);

		// Takes the abstract proc gen layers created so far and creates a concrete chunk
		void finalizeBuild(sf::Vector2i coordinate, entt::registry& registry) const;

		sf::Vector2i getDimensions() const;

		gen::BiomeIcon getBiomeIcon(sf::Vector2i coordinate) const;
		

	private:


	private:
		sf::Vector2i _dimensions;
		std::unordered_map<sf::Vector2i, std::string> _coordinates;
		gen::WorldGenerator _worldGenerator;
	};
}


