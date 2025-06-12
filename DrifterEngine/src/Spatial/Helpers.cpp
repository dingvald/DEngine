#include "pch.h"
#include "Helpers.h"
#include "Utility/stdHashing.h"
#include <Spatial/Conversions.h>

static const sf::Vector3i ZeroVector3i = { 0,0,0 };

drft::spatial::OffsetPosition drft::spatial::collapseOffset(const TilePosition& position, const sf::Vector3f& offset)
{
	TilePosition tile = toTileSpace(offset);
	sf::Vector3f remainder = offset - toFloatSpace(tile);
	
	return { position + tile, remainder};
}

sf::Vector3i drft::spatial::vec3FromPlanar(sf::Vector2i plane)
{
	return { plane.x, plane.y, 0 };
}

sf::Vector2i drft::spatial::toXY(sf::Vector3i vec3)
{
	return { vec3.x, vec3.y };
}

sf::Vector2f drft::spatial::toXY(sf::Vector3f vec3)
{
	return { vec3.x, vec3.y };
}

bool drft::spatial::isAtChunkOrigin(TilePosition position)
{
	auto chunkLocalPosition = toChunkLocalSpace(position);
	return chunkLocalPosition == ZeroVector3i;
}

std::vector<sf::Vector2i> drft::spatial::getIntRect(sf::Vector2i origin, int width, int height)
{
	std::vector<sf::Vector2i> result;
	result.reserve(width * height);
	for (int y = origin.y; y < origin.y + height; ++y)
	{
		for (int x = origin.x; x < origin.x + width; ++x)
		{
			result.emplace_back(x, y);
		}
	}

	return result;
}

std::vector<sf::Vector2i> drft::spatial::getOutlineIntRect(sf::Vector2i origin, int width, int height)
{
	std::vector<sf::Vector2i> result;
	result.reserve(2 * width + 2 * (height - 2));
	// top and bottom
	for (int x = 0; x < width; ++x)
	{
		result.push_back({ x, 0 });
		result.push_back({ x, height - 1 });
	}
	// left and right
	for (int y = 1; y < height - 1; ++y)
	{
		result.push_back({ 0, y });
		result.push_back({ width - 1, y });
	}

	return result;
}

std::vector<sf::Vector3i> drft::spatial::getIntCircleInRadius(sf::Vector3i centerPosition, int radius)
{
	std::vector<sf::Vector3i> result;
	size_t approxSquares = static_cast<size_t>(std::ceil(4 * radius * radius)); // Close enough to pi for reserve
	result.reserve(approxSquares);
	std::unordered_set<sf::Vector3i> visited;

	auto tryAdd = [&](int x, int y)
		{
			sf::Vector3i addedPos = sf::Vector3i{x, y, 0} + centerPosition;
			if (visited.contains(addedPos)) return;

			visited.insert(addedPos);
			result.emplace_back(std::move(addedPos));
		};

	for (int i = 0; i <= radius; i++)
	{
		for (int j = 0; j <= i; j++)
		{
			if (!isWithinRadius2d({ i, j }, radius)) continue;

			tryAdd(i, j);
			tryAdd(-i, j);
			tryAdd(i, -j);
			tryAdd(-i, -j);
			tryAdd(j, i);
			tryAdd(-j, i);
			tryAdd(j, -i);
			tryAdd(-j, -i);
		}
	}

	return result;
}

std::vector<sf::Vector3i> drft::spatial::getIntRectAroundOrigin(sf::Vector3i centerPosition, int width, int height)
{
	std::vector<sf::Vector3i> result;
	std::unordered_set<sf::Vector2i> visited;
	result.reserve(width * height);
	const int z = centerPosition.z;

	for (int x = centerPosition.x; x <= centerPosition.x + width / 2; x++)
	{
		for (int y = centerPosition.y; y <= centerPosition.y + height / 2; y++)
		{
			sf::Vector2i p1 = {  x,  y };
			sf::Vector2i p2 = { -x,  y };
			sf::Vector2i p3 = {  x, -y };
			sf::Vector2i p4 = { -x, -y };
			if (!visited.contains(p1))
			{
				result.emplace_back(p1.x, p1.y, z);
				visited.insert(p1);
			}
			if (!visited.contains(p2))
			{
				result.emplace_back(p2.x, p2.y, z);
				visited.insert(p2);
			}
			if (!visited.contains(p3))
			{
				result.emplace_back(p3.x, p3.y, z);
				visited.insert(p3);
			}
			if (!visited.contains(p4))
			{
				result.emplace_back(p4.x, p4.y, z);
				visited.insert(p4);
			}
		}
	}

	return result;
}

std::vector<sf::Vector2i> drft::spatial::getLine2d(sf::Vector2i pt1, sf::Vector2i pt2)
{
	// Bresenham's algorithm
	std::vector<sf::Vector2i> result;
	result.reserve(static_cast<size_t>(spatial::distance2d(pt1, pt2)));

	int dx = std::abs(pt2.x - pt1.x);
	int dy = -std::abs(pt2.y - pt1.y);
	int sx = pt1.x < pt2.x ? 1 : -1;
	int sy = pt1.y < pt2.y ? 1 : -1;
	int x = pt1.x;
	int y = pt1.y;
	int err = dx + dy;
	int e2;

	while (true)
	{
		if (x == pt2.x && y == pt2.y) break;
		e2 = 2 * err;
		if (e2 >= dy)
		{
			err += dy;
			x += sx;
		}
		if (e2 <= dx)
		{
			err += dx;
			y += sy;
		}
		result.emplace_back(x, y);
	}
	
	return result;
}

std::vector<sf::Vector3i> drft::spatial::getLine3d(sf::Vector3i pt1, sf::Vector3i pt2)
{
	std::vector<sf::Vector3i> result;

	result.push_back(pt1);
	const int dx = std::abs(pt2.x - pt1.x);
	const int dy = std::abs(pt2.y - pt1.y);
	const int dz = std::abs(pt2.z - pt1.z);
	int xs;
	int ys;
	int zs;
	if (pt2.x > pt1.x)
		xs = 1;
	else
		xs = -1;
	if (pt2.y > pt1.y)
		ys = 1;
	else
		ys = -1;
	if (pt2.z > pt1.z)
		zs = 1;
	else
		zs = -1;

	if (dx >= dy && dx >= dz) {
		int p1 = 2 * dy - dx;
		int p2 = 2 * dz - dx;
		while (pt1.x != pt2.x) {
			pt1.x += xs;
			if (p1 >= 0) {
				pt1.y += ys;
				p1 -= 2 * dx;
			}
			if (p2 >= 0) {
				pt1.z += zs;
				p2 -= 2 * dx;
			}
			p1 += 2 * dy;
			p2 += 2 * dz;
			result.push_back(pt1);
		}
	}
	else if (dy >= dx && dy >= dz) {
		int p1 = 2 * dx - dy;
		int p2 = 2 * dz - dy;
		while (pt1.y != pt2.y) {
			pt1.y += ys;
			if (p1 >= 0) {
				pt1.x += xs;
				p1 -= 2 * dy;
			}
			if (p2 >= 0) {
				pt1.z += zs;
				p2 -= 2 * dy;
			}
			p1 += 2 * dx;
			p2 += 2 * dz;
			result.push_back(pt1);
		}
	}
	else {
		int p1 = 2 * dy - dz;
		int p2 = 2 * dx - dz;
		while (pt1.z != pt2.z) {
			pt1.z += zs;
			if (p1 >= 0) {
				pt1.y += ys;
				p1 -= 2 * dz;
			}
			if (p2 >= 0) {
				pt1.x += xs;
				p2 -= 2 * dz;
			}
			p1 += 2 * dy;
			p2 += 2 * dx;
			result.push_back(pt1);
		}
	}

	return result;
}

float drft::spatial::distance2d(sf::Vector2i pt1, sf::Vector2i pt2)
{
	const auto delta = pt1 - pt2;
	return std::hypotf(static_cast<float>(delta.x), static_cast<float>(delta.y));
}

float drft::spatial::distance2d(sf::Vector2f pt1, sf::Vector2f pt2)
{
	const auto delta = pt1 - pt2;
	return std::hypotf(delta.x, delta.y);
}

float drft::spatial::distance3d(sf::Vector3i pt1, sf::Vector3i pt2)
{
	const float dx = static_cast<float>(pt2.x - pt1.x);
	const float dy = static_cast<float>(pt2.y - pt1.y);
	const float dz = static_cast<float>(pt2.z - pt1.z);
	return std::sqrtf(dx*dx + dy*dy + dz*dz);
}

float drft::spatial::distance3d(sf::Vector3f pt1, sf::Vector3f pt2)
{
	const float dx = pt2.x - pt1.x;
	const float dy = pt2.y - pt1.y;
	const float dz = pt2.z - pt1.z;
	return std::sqrtf(dx * dx + dy * dy + dz * dz);
}

bool drft::spatial::isWithinRadius2d(sf::Vector2i origin, sf::Vector2i point, int radius)
{
	int normalized_x = point.x - origin.x;
	int normalized_y = point.y - origin.y;
	return (normalized_x*normalized_x) + (normalized_y*normalized_y) <= (radius*radius);
}

bool drft::spatial::isWithinRadius2d(sf::Vector2f origin, sf::Vector2f point, float radius)
{
	float normalized_x = point.x - origin.x;
	float normalized_y = point.y - origin.y;
	return (normalized_x * normalized_x) + (normalized_y * normalized_y) <= (radius * radius);
}

bool drft::spatial::isWithinRadius2d(sf::Vector2i point, int radius)
{
	return isWithinRadius2d({ 0,0 }, point, radius);
}

bool drft::spatial::isWithinRadius2d(sf::Vector2f point, float radius)
{
	return isWithinRadius2d({ 0.f, 0.f }, point, radius);
}

bool drft::spatial::isWithinRadius3d(sf::Vector3i origin, sf::Vector3i point, int radius)
{
	const int dx = point.x - origin.x;
	const int dy = point.y - origin.y;
	const int dz = point.z - origin.z;
	return dx*dx + dy*dy + dz*dz <= radius*radius;
}

bool drft::spatial::isWithinRadius3d(drft::TilePosition origin, drft::TilePosition point, int radius)
{
	const int dx = point.x - origin.x;
	const int dy = point.y - origin.y;
	const int dz = point.z - origin.z;
	return dx * dx + dy * dy + dz * dz <= radius * radius;
}

bool drft::spatial::isWithinRadius3d(sf::Vector3f origin, sf::Vector3f point, float radius)
{
	const float dx = point.x - origin.x;
	const float dy = point.y - origin.y;
	const float dz = point.z - origin.z;
	return dx * dx + dy * dy + dz * dz <= radius * radius;
}

bool drft::spatial::isWithinRadius3d(sf::Vector3i point, int radius)
{
	return isWithinRadius3d({ 0,0,0 }, point, radius);
}

bool drft::spatial::isWithinRadius3d(sf::Vector3f point, float radius)
{
	return isWithinRadius3d({ 0,0,0 }, point, radius);
}

sf::Vector2i drft::spatial::findClosestPoint2d(sf::Vector2i target, const std::vector<sf::Vector2i>& points)
{
	float minDistance = std::numeric_limits<float>::max();
	sf::Vector2i result = target;

	for (auto&& point : points)
	{
		float distance = spatial::distance2d(target, point);
		if (distance < minDistance)
		{
			result = point;
			minDistance = distance;
		}
	}
	return result;
}

sf::Vector3i drft::spatial::findClosestPoint3d(sf::Vector3i target, const std::vector<sf::Vector3i>& points)
{
	float minDistance = std::numeric_limits<float>::max();
	sf::Vector3i result = target;

	for (auto&& point : points)
	{
		float distance = spatial::distance3d(target, point);
		if (distance < minDistance)
		{
			result = point;
			minDistance = distance;
		}
	}
	return result;
}

std::vector<sf::Vector2i> drft::spatial::getAdjacentPoints(sf::Vector2i point, AdjacentType type)
{
	constexpr auto ordinals = [](sf::Vector2i point) -> std::vector<sf::Vector2i>
	{
		std::vector<sf::Vector2i> result =
		{
			{point + sf::Vector2i{-1,-1}},
			{point + sf::Vector2i{-1, 1}},
			{point + sf::Vector2i{ 1,-1}},
			{point + sf::Vector2i{ 1, 1}}
		};
		return result;
	};
	constexpr auto cardinals = [](sf::Vector2i point) -> std::vector<sf::Vector2i>
	{
		std::vector<sf::Vector2i> result =
		{
			{point + sf::Vector2i{-1, 0}},
			{point + sf::Vector2i{ 0, 1}},
			{point + sf::Vector2i{ 1, 0}},
			{point + sf::Vector2i{ 0,-1}}
		};
		return result;
	};
	constexpr auto both = [](sf::Vector2i point) -> std::vector<sf::Vector2i>
	{
		std::vector<sf::Vector2i> result =
		{
			{point + sf::Vector2i{-1, 0}},
			{point + sf::Vector2i{ 0, 1}},
			{point + sf::Vector2i{ 1, 0}},
			{point + sf::Vector2i{ 0,-1}},
			{point + sf::Vector2i{-1,-1}},
			{point + sf::Vector2i{-1, 1}},
			{point + sf::Vector2i{ 1,-1}},
			{point + sf::Vector2i{ 1, 1}}
		};
		return result;
	};

	switch (type)
	{
	case drft::spatial::AdjacentType::Ordinal:
		return ordinals(point);
		break;
	case drft::spatial::AdjacentType::Cardinal:
		return cardinals(point);
		break;
	case drft::spatial::AdjacentType::OrdinalCardinal:
		return both(point);
		break;
	default:
		throw std::exception("Unhandled enum type.");
		break;
	}
	return {};
}

std::vector<sf::Vector2i> drft::spatial::getPointDeltas(sf::Vector2i point, const std::vector<sf::Vector2i>& points)
{
	std::vector<sf::Vector2i> result;
	result.reserve(points.size());
	for (auto pt : points)
	{
		result.push_back(pt - point);
	}
	return result;
}

std::vector<sf::Vector3i> drft::spatial::getSurroundingPoints(sf::Vector3i point, PlaneType plane)
{
	switch (plane)
	{
	case drft::spatial::PlaneType::XY:
		return
		{
			{point + sf::Vector3i{-1, 0, 0}},
			{point + sf::Vector3i{ 0, 1, 0}},
			{point + sf::Vector3i{ 1, 0, 0}},
			{point + sf::Vector3i{ 0,-1, 0}},
			{point + sf::Vector3i{-1,-1, 0}},
			{point + sf::Vector3i{-1, 1, 0}},
			{point + sf::Vector3i{ 1,-1, 0}},
			{point + sf::Vector3i{ 1, 1, 0}}
		};
		break;
	case drft::spatial::PlaneType::XZ:
		return
		{
			{point + sf::Vector3i{-1, 0, 0}},
			{point + sf::Vector3i{ 0, 0, 1}},
			{point + sf::Vector3i{ 1, 0, 0}},
			{point + sf::Vector3i{ 0,0, -1}},
			{point + sf::Vector3i{-1,0, -1}},
			{point + sf::Vector3i{-1, 0, 1}},
			{point + sf::Vector3i{ 1,0, -1}},
			{point + sf::Vector3i{ 1, 0, 1}}
		};
		break;
	case drft::spatial::PlaneType::YZ:
		return
		{
			{point + sf::Vector3i{0, -1, 0}},
			{point + sf::Vector3i{ 0, 0, 1}},
			{point + sf::Vector3i{ 0, 1, 0}},
			{point + sf::Vector3i{ 0,0, -1}},
			{point + sf::Vector3i{0,-1, -1}},
			{point + sf::Vector3i{0, -1, 1}},
			{point + sf::Vector3i{ 0,1, -1}},
			{point + sf::Vector3i{ 0, 1, 1}}
		};
		break;
	case drft::spatial::PlaneType::All:
		return 
		{
		point + sf::Vector3i(1,  1,  1), point + sf::Vector3i(1,  1,  0), point + sf::Vector3i(1,  1, -1),
		point + sf::Vector3i(1,  0,  1), point + sf::Vector3i(1,  0,  0), point + sf::Vector3i(1,  0, -1),
		point + sf::Vector3i(1, -1,  1), point + sf::Vector3i(1, -1,  0), point + sf::Vector3i(1, -1, -1),

		point + sf::Vector3i(0,  1,  1), point + sf::Vector3i(0,  1,  0), point + sf::Vector3i(0,  1, -1),
		point + sf::Vector3i(0,  0,  1), point + sf::Vector3i(0,  0,  0), point + sf::Vector3i(0,  0, -1),
		point + sf::Vector3i(0, -1,  1), point + sf::Vector3i(0, -1,  0), point + sf::Vector3i(0, -1, -1),

		point + sf::Vector3i(-1,  1,  1), point + sf::Vector3i(-1,  1,  0), point + sf::Vector3i(-1,  1, -1),
		point + sf::Vector3i(-1,  0,  1), point + sf::Vector3i(-1,  0,  0), point + sf::Vector3i(-1,  0, -1),
		point + sf::Vector3i(-1, -1,  1), point + sf::Vector3i(-1, -1,  0), point + sf::Vector3i(-1, -1, -1)
		};
		break;
	default:
		break;
	}
	return {};
}

void drft::spatial::forEachPointInRect(sf::IntRect rect, std::function<void(sf::Vector2i)> func)
{
	for (int y = rect.position.y; y < rect.position.y + rect.size.y; ++y)
	{
		for (int x = rect.position.x; x < rect.position.x + rect.size.y; ++x)
		{
			func(sf::Vector2i{ x, y });
		}
	}
}

void drft::spatial::forEachPointInVolume(drft::spatial::AABB<int> volume, std::function<void(sf::Vector3i)> func)
{
	for (int x = volume.min.x; x < volume.max.x; x++)
	{
		for (int y = volume.min.y; y < volume.max.y; y++)
		{
			for (int z = volume.min.z; z < volume.max.z; z++)
			{
				func(sf::Vector3i{ x,y,z });
			}
		}
	}
}


