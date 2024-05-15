#pragma once
#include "Spatial/AutoGrid.h"
#include "Utility/stdHashing.h"

namespace drft
{
	enum class StructureBit : size_t
	{
		Reserved,
		Wall,
		Floor,
		Room,
		Door
	};

	class StructureShapeInstance
	{	
	public:
		void addEntity(const std::string& name, sf::Vector2i position);
		void clearPosition(sf::Vector2i position);
		const std::unordered_map<std::string, std::vector<sf::Vector2i>>& getEntityPositions() const;

		bool checkBit(sf::Vector2i position, StructureBit bit) const;
		void setBit(sf::Vector2i position, StructureBit bit);
		void clearBit(sf::Vector2i position, StructureBit bit);
		bool hasBit(StructureBit bit) const;
		const std::unordered_set<sf::Vector2i>& getPositionsFor(StructureBit bit) const;
	private:
		spatial::AutoGrid<std::bitset<8u>> _bitGrid;
		std::unordered_map<StructureBit, std::unordered_set<sf::Vector2i>> _bitPositions;
		std::unordered_map<std::string, std::vector<sf::Vector2i>> _entityPositions;
	};
}


