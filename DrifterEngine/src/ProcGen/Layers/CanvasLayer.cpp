#include "pch.h"
#include "CanvasLayer.h"

using namespace entt::literals;

const int CanvasLayer::DefaultPriority = 0;
const int CanvasLayer::UninitializedPriority = std::numeric_limits<int>::min();

void CanvasLayer::set(std::any val, sf::Vector3i position, int priority)
{
	setOrReplace(val, position, priority);
}

std::any CanvasLayer::get(sf::Vector3i position) const
{
	return _grid.at(position).value;
}

void CanvasLayer::discard(sf::Vector3i position)
{
	_grid.discard(position);
}

void CanvasLayer::setOrReplace(std::any val, sf::Vector3i position, int priority)
{
	const int currentPriority = _grid.at(position).priority;
	if (priority >= currentPriority)
	{
		_grid.at(position).priority = priority;
		_grid.at(position).value = val;
	}
}
