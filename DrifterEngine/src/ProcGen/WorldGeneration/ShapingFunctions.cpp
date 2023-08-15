#include "pch.h"
#include "ShapingFunctions.h"
#include "Spatial/Helpers.h"
#include "Utility/Math.h"

void drft::gen::setCircle(sf::Vector2i origin, int radius, double val, float dropoff, spatial::Grid<double>& map)
{
	auto positions = spatial::getIntCircleInRadius(origin, radius);
	for (auto position : positions)
	{
		if (!map.contains(position.x, position.y)) continue;
		double addedValue = val - (spatial::distance(origin, position) * dropoff);
		map.at(position.x, position.y) += addedValue;
	}
}

void drft::gen::setDropOffCircle(sf::Vector2i origin, double val, float dropoff, spatial::Grid<double>& map)
{
	int radius = val / dropoff;
	setCircle(origin, radius, val, dropoff, map);
}

void drft::gen::setDropOffRect(sf::IntRect area, double val, float dropoff, spatial::Grid<double>& map)
{
	for (int y = area.top; y < area.height + area.top; ++y)
	{
		for (int x = area.left; x < area.width + area.left; ++x)
		{
			float distance = spatial::distance(sf::Vector2f(x, y), sf::Vector2f(x, 0));
			double calcedVal = val - dropoff * distance;
			map.at(x, y) = std::clamp(map.at(x, y) + calcedVal, 0.0, 1.0);
		}
	}
}

void drft::gen::remap(double iMin, double iMax, double oMin, double oMax, spatial::Grid<double>& map)
{
	for (int y = 0; y < map.height(); ++y)
	{
		for (int x = 0; x < map.width(); ++x)
		{
			map.at(x, y) = math::remap(iMin, iMax, oMin, oMax, map.at(x, y));
		}
	}
}

