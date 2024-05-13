#pragma once
#include "Spatial/AutoGrid.h"

namespace drft
{
	using PositionList = std::vector<sf::Vector2i>;
	using LayoutPtr = std::unique_ptr<std::unordered_map<std::string, PositionList>>;

	

	class StructureBaseShape
	{
	public:
		virtual void createFromJSON(const rapidjson::Value& json) = 0;
		virtual void generateLayout() = 0;
		LayoutPtr extractLayout();

	protected:
		enum StructureBit : size_t
		{
			Wall,
			Floor
		};
		void AddToLayout(const std::string& entityName, sf::Vector2i position);
		bool checkBit(sf::Vector2i position, StructureBit bit) const;
		void setBit(sf::Vector2i position, StructureBit bit);
		void clearBit(sf::Vector2i position, StructureBit bit);

	private:
		LayoutPtr _layout;
		spatial::AutoGrid<std::bitset<8u>> _bitGrid;
	};
}

