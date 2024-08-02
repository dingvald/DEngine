#include "pch.h"
#include "StructureShapeInstance.h"

void drft::StructureShapeInstance::addEntity(const std::string& name, sf::Vector2i position)
{
	_entityPositions[name].push_back(position);
}

void drft::StructureShapeInstance::removeEntities(sf::Vector2i position)
{
	for (auto&& [_, positions] : _entityPositions)
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

void drft::StructureShapeInstance::clearPosition(sf::Vector2i position)
{
	_tagGrid.at(position.x, position.y).clear();
	for (auto&& [bit, positions] : _tagPositions)
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

bool drft::StructureShapeInstance::hasAnyTag(sf::Vector2i position) const
{
	return !_tagGrid.at(position.x, position.y).empty();
}

bool drft::StructureShapeInstance::hasTag(entt::id_type tag, sf::Vector2i position) const
{
	return _tagGrid.at(position.x, position.y).contains(tag);
}

void drft::StructureShapeInstance::setTag(entt::id_type tag, sf::Vector2i position)
{
	_tagGrid.at(position.x, position.y).insert(tag);
	_tagPositions[tag].insert(position);
}

void drft::StructureShapeInstance::clearTag(entt::id_type tag, sf::Vector2i position)
{
	_tagGrid.at(position.x, position.y).erase(tag);
	if (hasTag(tag))
	{
		_tagPositions.at(tag).erase(position);
	}
}

bool drft::StructureShapeInstance::hasTag(entt::id_type type) const
{
	return _tagPositions.contains(type);
}

const entt::dense_set<sf::Vector2i>& drft::StructureShapeInstance::getPositionsFor(entt::id_type type) const
{
	return _tagPositions.at(type);
}
