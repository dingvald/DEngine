#include "pch.h"
#include "Helpers.h"
#include "Utility/stdHashing.h"

namespace 
{
	std::vector<sf::Vector2i> getEightWaySymmetry(int x, int y)
	{
		return {
				{x, y},
				{y, x},
				{-x, y},
				{-x, -y},
				{x, -y},
				{-y, x},
				{-y, -x},
				{y, -x}
		};
	}
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

std::vector<sf::Vector2i> drft::spatial::getIntCircleInRadius(sf::Vector2i centerPosition, int radius)
{
	std::vector<sf::Vector2i> result;
	std::unordered_set<sf::Vector2i> visited;
	size_t approxSquares = static_cast<size_t>(std::ceil(3.5 * radius * radius));
	result.reserve(approxSquares);

	for (int i = 0; i <= radius; i++)
	{
		for (int j = 0; j <= i; j++)
		{
			if (!isWithinRadius({ i, j }, radius)) continue;
			for (auto&& p : getEightWaySymmetry(i, j))
			{
				if (visited.contains(p)) continue;
				result.push_back(centerPosition + p);
				visited.insert(p);
			}
		}
	}

	return result;
}

std::vector<sf::Vector2i> drft::spatial::getIntRectAroundOrigin(sf::Vector2i origin, int width, int height)
{
	std::vector<sf::Vector2i> result;
	result.reserve(width * height);
	for (int y = origin.y - height/2; y <= origin.y + height/2; ++y)
	{
		for (int x = origin.x - width/2; x <= origin.x + width/2; ++x)
		{
			result.emplace_back(x, y);
		}
	}

	return result;
}

std::vector<sf::Vector2i> drft::spatial::getIntPointsAlongLine(sf::Vector2i pt1, sf::Vector2i pt2)
{
	// Bresenham's algorithm
	std::vector<sf::Vector2i> result;
	result.reserve(static_cast<size_t>(spatial::distance(pt1, pt2)));

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

float drft::spatial::distance(sf::Vector2i pt1, sf::Vector2i pt2)
{
	const auto delta = pt1 - pt2;
	return std::hypotf(static_cast<float>(delta.x), static_cast<float>(delta.y));
}

float drft::spatial::distance(sf::Vector2f pt1, sf::Vector2f pt2)
{
	const auto delta = pt1 - pt2;
	return std::hypotf(delta.x, delta.y);
}

bool drft::spatial::isWithinRadius(sf::Vector2i origin, sf::Vector2i point, int radius)
{
	int normalized_x = point.x - origin.x;
	int normalized_y = point.y - origin.y;
	return (normalized_x*normalized_x) + (normalized_y*normalized_y) <= (radius*radius);
}

bool drft::spatial::isWithinRadius(sf::Vector2f origin, sf::Vector2f point, float radius)
{
	float normalized_x = point.x - origin.x;
	float normalized_y = point.y - origin.y;
	return (normalized_x * normalized_x) + (normalized_y * normalized_y) <= (radius * radius);
}

bool drft::spatial::isWithinRadius(sf::Vector2i point, int radius)
{
	return isWithinRadius({ 0,0 }, point, radius);
}

bool drft::spatial::isWithinRadius(sf::Vector2f point, float radius)
{
	return isWithinRadius({ 0.f, 0.f }, point, radius);
}

sf::Vector2i drft::spatial::findClosestPoint(sf::Vector2i target, const std::vector<sf::Vector2i>& points)
{
	float minDistance = std::numeric_limits<float>::max();
	sf::Vector2i result = target;

	for (auto&& point : points)
	{
		float distance = spatial::distance(target, point);
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

void drft::spatial::forEachPointInRect(sf::IntRect rect, std::function<void(sf::Vector2i)> func)
{
	for (int y = rect.top; y < rect.top + rect.height; ++y)
	{
		for (int x = rect.left; x < rect.left + rect.width; ++x)
		{
			func(sf::Vector2i{ x, y });
		}
	}
}


