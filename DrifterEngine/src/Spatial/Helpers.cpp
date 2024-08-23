#include "pch.h"
#include "Helpers.h"
#include "Utility/stdHashing.h"

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

std::vector<sf::Vector2i> drft::spatial::getIntCircleInRadius(const sf::Vector2i centerPosition, const int radius)
{
	std::vector<sf::Vector2i> result;
	float approxSquares = std::ceil(3.13159 * radius * radius);
	result.reserve(static_cast<size_t>(approxSquares));

	for (int y = centerPosition.y - radius; y < centerPosition.y; ++y)
	{
		for (int x = centerPosition.x - radius; x < centerPosition.x; ++x)
		{
			if ((x - centerPosition.x) * (x - centerPosition.x) + (y - centerPosition.y) * (y - centerPosition.y) <= radius * radius)
			{
				int xMirror = centerPosition.x - (x - centerPosition.x);
				int yMirror = centerPosition.y - (y - centerPosition.y);

				result.insert(result.end(), { {x, y}, {x, yMirror}, {xMirror, y}, {xMirror, yMirror} });
			}
		}
	}
	// To prevent duplicates along the center points..
	for (int i = -radius; i <= radius; ++i)
	{
		if (i != 0)
		{
			result.push_back({ centerPosition.x + i, centerPosition.y });
		}
		result.push_back({ centerPosition.x, centerPosition.y + i });
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

std::optional<sf::Vector2i> drft::spatial::findClosestPoint(sf::Vector2i target, const std::vector<sf::Vector2i>& points)
{
	if (points.empty()) return std::nullopt;

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


