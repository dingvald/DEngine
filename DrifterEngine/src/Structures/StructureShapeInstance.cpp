#include "pch.h"
#include "StructureShapeInstance.h"

void drft::StructureShapeInstance::addEntity(const std::string& name, sf::Vector2i position)
{
	_entityPositions[name].push_back(position);
}

void drft::StructureShapeInstance::clearPosition(sf::Vector2i position)
{
	_bitGrid.at(position.x, position.y).reset();
	for (auto&& [bit, positions] : _bitPositions)
	{
		positions.erase(position);
	}
	for (auto&& [entityName, positions] : _entityPositions)
	{
		auto it = positions.begin();
		while (it != positions.end())
		{
			if (*it == position)
			{
				it = positions.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

const std::unordered_map<std::string, std::vector<sf::Vector2i>>& drft::StructureShapeInstance::getEntityPositions() const
{
	return _entityPositions;
}

bool drft::StructureShapeInstance::checkBit(sf::Vector2i position, StructureBit bit) const
{
	return _bitGrid.at(position.x, position.y).test((size_t)bit);
}

void drft::StructureShapeInstance::setBit(sf::Vector2i position, StructureBit bit)
{
	_bitGrid.at(position.x, position.y).set((size_t)bit);
	_bitPositions[bit].insert(position);
}

void drft::StructureShapeInstance::clearBit(sf::Vector2i position, StructureBit bit)
{
	_bitGrid.at(position.x, position.y).set((size_t)bit, false);
	if (hasBit(bit))
	{
		_bitPositions.at(bit).erase(position);
	}
}

bool drft::StructureShapeInstance::hasBit(StructureBit bit) const
{
	return _bitPositions.contains(bit);
}

const std::unordered_set<sf::Vector2i>& drft::StructureShapeInstance::getPositionsFor(StructureBit bit) const
{
	return _bitPositions.at(bit);
}
