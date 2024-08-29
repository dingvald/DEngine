#pragma once

namespace drft::spatial
{
	std::vector<sf::Vector2i> getIntRect(sf::Vector2i origin, int width, int height);
	std::vector<sf::Vector2i> getOutlineIntRect(sf::Vector2i origin, int width, int height);
	// Returns a list of integer points located in a circle around the given center.
	std::vector<sf::Vector2i> getIntCircleInRadius(sf::Vector2i centerPosition, int radius);
	// Returns a list of integer points located in a rect around the given origin.
	std::vector<sf::Vector2i> getIntRectAroundOrigin(sf::Vector2i origin, int width, int height);
	// Returns a list of integer points located along a line.
	std::vector<sf::Vector2i> getIntPointsAlongLine(sf::Vector2i pt1, sf::Vector2i pt2);

	// Returns the distance bewteen two points.
	inline float distance(sf::Vector2i pt1, sf::Vector2i pt2);
	// Returns the distance bewteen two points.
	inline float distance(sf::Vector2f pt1, sf::Vector2f pt2);
	// Tests if a point is within some radius from the origin
	inline bool isWithinRadius(sf::Vector2i origin, sf::Vector2i point, int radius);
	// Tests if a point is within some radius from the origin
	inline bool isWithinRadius(sf::Vector2f origin, sf::Vector2f point, float radius);
	// Tests if a point is within some radius from (0,0)
	inline bool isWithinRadius(sf::Vector2i point, int radius);
	// Tests if a point is within some radius from (0,0)
	inline bool isWithinRadius(sf::Vector2f point, float radius);

	sf::Vector2i findClosestPoint(sf::Vector2i target, const std::vector<sf::Vector2i>& points);

	enum class AdjacentType
	{
		Ordinal,
		Cardinal,
		OrdinalCardinal
	};
	// Returns a list of integer points touching a given integer point.
	// Cardinal: N, S, E, W	
	// Ordinal: NE, NW, SE, SW
	std::vector<sf::Vector2i> getAdjacentPoints(sf::Vector2i point, AdjacentType type = AdjacentType::OrdinalCardinal);
	std::vector<sf::Vector2i> getPointDeltas(sf::Vector2i point, const std::vector<sf::Vector2i>& points);

	void forEachPointInRect(sf::IntRect rect, std::function<void(sf::Vector2i point)> func);
}


