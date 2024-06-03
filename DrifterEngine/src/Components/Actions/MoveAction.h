#pragma once
#include <SFML/System/Vector2.hpp>

struct PerformMoveAction
{
	sf::Vector2i direction = { 0,0 };
};

struct TryMoveAction
{
	sf::Vector2i direction = { 0,0 };
};

struct DoMoveAction
{
	sf::Vector2i direction = { 0,0 };
};