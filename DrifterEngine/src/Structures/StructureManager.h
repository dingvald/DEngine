#pragma once
#include "Structures/StructureFactory.h"
#include "Structures/StructureInstance.h"

namespace drft
{
	namespace gen
	{
		class WorldGenerator;
	}
	
	class StructureManager
	{
	public:
		StructureManager(const gen::WorldGenerator& generator);

		void scanForStuctures(sf::Vector2i origin);
		const StructureInstance* getStructureAt(sf::Vector2i coordinate) const;

	private:


	private:
		const gen::WorldGenerator& _generator;
		StructureFactory _structureFactory;
		std::unordered_map<sf::Vector2i, StructureInstancePtr> _structures;
		std::unordered_set<sf::Vector2i> _visited;
	};

}

