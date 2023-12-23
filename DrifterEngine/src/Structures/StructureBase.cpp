#include "pch.h"
#include "StructureBase.h"

sf::Vector2i drft::StructureBase::getMaximumBounds() const
{
	return _maxBounds;
}

sf::Vector2i drft::StructureBase::getMinimumBounds() const
{
	return _minBounds;
}

const std::string& drft::StructureBase::name() const
{
	return _name;
}

void drft::StructureBase::setMaximumBounds(sf::Vector2i bounds)
{
	_maxBounds = bounds;
}

void drft::StructureBase::setMinimumBounds(sf::Vector2i bounds)
{
	_minBounds = bounds;
}

void drft::StructureBase::addSubstructure(std::string name, sf::Vector2i relativePosition)
{
	_substructures.emplace_back(std::move(name), relativePosition);
}
