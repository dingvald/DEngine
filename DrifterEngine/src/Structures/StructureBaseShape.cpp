#include "pch.h"
#include "StructureBaseShape.h"

drft::LayoutPtr drft::StructureBaseShape::extractLayout()
{
	return std::move(_layout);
}

void drft::StructureBaseShape::AddToLayout(const std::string& entityName, sf::Vector2i position)
{
	(*_layout)[entityName].push_back(position);
}

bool drft::StructureBaseShape::checkBit(sf::Vector2i position, StructureBit bit) const
{
	return _bitGrid.at(position.x, position.y).test(bit);
}

void drft::StructureBaseShape::setBit(sf::Vector2i position, StructureBit bit)
{
	_bitGrid.at(position.x, position.y).set(bit);
}

void drft::StructureBaseShape::clearBit(sf::Vector2i position, StructureBit bit)
{
	_bitGrid.at(position.x, position.y).set(bit, false);
}
