#pragma once

namespace drft::spatial
{
	// Returns a list of integer points located in a circle around the given center.
	std::vector<sf::Vector2i> getIntCircleInRadius(sf::Vector2i centerPosition, int radius);
	// Returns a list of integer points located in a rect around the given origin.
	std::vector<sf::Vector2i> getIntRectAroundOrigin(sf::Vector2i origin, int width, int height);
	// Returns a list of integer points located along a line.
	std::vector<sf::Vector2i> getIntPointsAlongLine(sf::Vector2i pt1, sf::Vector2i pt2);
	// Returns the distance bewteen two points.
	float distance(sf::Vector2i pt1, sf::Vector2i pt2);
	// Returns the distance bewteen two points.
	float distance(sf::Vector2f pt1, sf::Vector2f pt2);
}


