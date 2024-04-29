#pragma once
#include "ProcGen/WorldGeneration/WorldGenerator.h"
#include "Utility/stdHashing.h"

namespace drft
{
	class WorldMap
	{
	public:
		// Takes the abstract proc gen layers created so far and creates a concrete chunk
		void finalizeChunk(sf::Vector2i coordinate, entt::registry& registry) const;

		void create();

		void load(cereal::JSONInputArchive& iarchive);
		void save(cereal::JSONOutputArchive& oarchive) const;

		void fixedUpdate(const entt::registry& registry);

		sf::Vector2i getDimensions() const;
		gen::BiomeIcon getBiomeIcon(sf::Vector2i coordinate) const;
		sf::Vector2i getStartingPosition(const std::string& biomeType) const;

	private:
		sf::Vector2i _dimensions;
		std::unordered_map<sf::Vector2i, std::string> _coordinates;
		gen::WorldGenerator _worldGenerator;
	};
}


