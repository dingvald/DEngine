#pragma once

namespace drft::algo
{
	std::vector<sf::Vector2i> floodFill(sf::Vector2i startingPosition, std::function<bool(sf::Vector2i)> isSameFunc);
}