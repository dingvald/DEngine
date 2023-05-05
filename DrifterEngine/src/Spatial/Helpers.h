#pragma once

namespace drft::spatial
{
	// Returns a list of integer points that are located around the given center, within the given radius.
	std::vector<sf::Vector2i> getIntCircleInRadius(sf::Vector2i centerPosition, int radius);

	std::vector<sf::Vector2i> getIntSquareInRadius(sf::Vector2i centerPosition, int radius);
	// Returns the distance bewteen two points.
	float distance(sf::Vector2i pt1, sf::Vector2i pt2);
	// Returns the distance bewteen two points.
	float distance(sf::Vector2f pt1, sf::Vector2f pt2);
}


