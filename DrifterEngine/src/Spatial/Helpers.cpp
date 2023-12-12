#include "pch.h"
#include "Helpers.h"
#include "Utility/stdHashing.h"

std::vector<sf::Vector2i> drft::spatial::getIntRect(sf::Vector2i origin, int width, int height)
{
	std::vector<sf::Vector2i> result;

	for (int y = origin.y; y <= origin.y + height; ++y)
	{
		for (int x = origin.x; x <= origin.x + width; ++x)
		{
			result.emplace_back(x, y);
		}
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
