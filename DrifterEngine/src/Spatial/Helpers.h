#pragma once
#include <Spatial/AABB.h>
#include <Spatial/TilePosition.h>

namespace drft::spatial
{
	struct OffsetPosition
	{
		TilePosition position;
		sf::Vector3f offset;
	};

	OffsetPosition collapseOffset(const TilePosition& position, const sf::Vector3f& offset);

	template<typename T>
	sf::Rect<T> expandToFit(const sf::Rect<T>& rect, const sf::Vector2<T>& point)
	{
		sf::Rect<T> result = rect;
		if (rect.contains(point)) return result;

		if (point.x < rect.position.x)
		{
			T left = rect.position.x + rect.size.x;
			result.position.x = point.x;
			result.size.x = left - result.position.x;
		}
		else if (point.x > rect.position.x + rect.size.x)
		{
			result.size.x = point.x - result.position.x;
		}

		if (point.y < rect.position.y)
		{
			T bottom = rect.position.y + rect.size.y;
			result.position.y = point.y;
			result.size.y = bottom - result.position.y;
		}
		else if (point.y > rect.position.y + rect.size.y)
		{
			result.size.y = point.y - result.position.y;
		}
	}

	// Returns a sf::Vector3i that copies the XY plane and assigns 0 to the Z dimension
	sf::Vector3i vec3FromPlanar(sf::Vector2i plane);
	sf::Vector2i toXY(sf::Vector3i vec3);
	sf::Vector2f toXY(sf::Vector3f vec3);

	std::vector<sf::Vector2i> getIntRect(sf::Vector2i origin, int width, int height);
	std::vector<sf::Vector2i> getOutlineIntRect(sf::Vector2i origin, int width, int height);
	// Returns a list of integer points located in a circle around the given center.
	std::vector<sf::Vector3i> getIntCircleInRadius(sf::Vector3i centerPosition, int radius);
	// Returns a list of integer points located in a rect around the given origin.
	std::vector<sf::Vector3i> getIntRectAroundOrigin(sf::Vector3i centerPosition, int width, int height);

	// Returns a list of integer points located along a line.
	std::vector<sf::Vector2i> getLine2d(sf::Vector2i pt1, sf::Vector2i pt2);
	// Returns a list of integer points located along a line.
	std::vector<sf::Vector3i> getLine3d(sf::Vector3i pt1, sf::Vector3i pt2);

	// Returns the distance bewteen two points.
	inline float distance2d(sf::Vector2i pt1, sf::Vector2i pt2);
	// Returns the distance bewteen two points.
	inline float distance2d(sf::Vector2f pt1, sf::Vector2f pt2);
	// Returns the distance bewteen two points.
	inline float distance3d(sf::Vector3i pt1, sf::Vector3i pt2);
	// Returns the distance bewteen two points.
	inline float distance3d(sf::Vector3f pt1, sf::Vector3f pt2);

	// Tests if a point is within some radius from the origin
	inline bool isWithinRadius2d(sf::Vector2i origin, sf::Vector2i point, int radius);
	// Tests if a point is within some radius from the origin
	inline bool isWithinRadius2d(sf::Vector2f origin, sf::Vector2f point, float radius);
	// Tests if a point is within some radius from (0,0)
	inline bool isWithinRadius2d(sf::Vector2i point, int radius);
	// Tests if a point is within some radius from (0,0)
	inline bool isWithinRadius2d(sf::Vector2f point, float radius);

	// Tests if a point is within some radius from the origin
	bool isWithinRadius3d(sf::Vector3i origin, sf::Vector3i point, int radius);
	// Tests if a point is within some radius from the origin
	bool isWithinRadius3d(TilePosition origin, TilePosition point, int radius);
	// Tests if a point is within some radius from the origin
	bool isWithinRadius3d(sf::Vector3f origin, sf::Vector3f point, float radius);
	// Tests if a point is within some radius from (0,0,0)
	bool isWithinRadius3d(sf::Vector3i point, int radius);
	// Tests if a point is within some radius from (0,0,0)
	bool isWithinRadius3d(sf::Vector3f point, float radius);

	sf::Vector2i findClosestPoint2d(sf::Vector2i target, const std::vector<sf::Vector2i>& points);
	sf::Vector3i findClosestPoint3d(sf::Vector3i target, const std::vector<sf::Vector3i>& points);

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

	enum class PlaneType
	{
		XY,
		XZ,
		YZ,
		All
	};
	std::vector<sf::Vector3i> getSurroundingPoints(sf::Vector3i point, PlaneType plane = PlaneType::All);

	void forEachPointInRect(sf::IntRect rect, std::function<void(sf::Vector2i)> func);
	void forEachPointInVolume(spatial::AABB<int> volume, std::function<void(sf::Vector3i)> func);
}


