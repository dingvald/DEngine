#include "pch.h"
#include "IAction.h"
#include "Spatial/Helpers.h"

bool drft::goap::IAction::isInRange(sf::Vector2i agentPosition, sf::Vector2i targetPosition) const
{
	float distance = spatial::distance(agentPosition, targetPosition);
	if (_range.isValueWithinInclusive(static_cast<int>(distance))) return true;
	return false;
}

const drft::goap::WorldState& drft::goap::IAction::getPreconditions() const
{
	return _preconditions;
}

const drft::goap::WorldState& drft::goap::IAction::getEffects() const
{
	return _effects;
}

void drft::goap::IAction::addPrecondition(const std::string& key, int val)
{
	_preconditions.add(key, val);
}

void drft::goap::IAction::addEffect(const std::string& key, int val)
{
	_preconditions.add(key, val);
}

void drft::goap::IAction::setRange(int min, int max)
{
	_range.min = min;
	_range.max = max;
}
