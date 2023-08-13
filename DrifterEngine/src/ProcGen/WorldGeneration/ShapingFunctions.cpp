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

void drft::gen::setRect(sf::Vector2i origin, sf::Vector2i dimensions, double val, sf::Vector2f dropoff, spatial::Grid<double>& map)
{
	auto positions = spatial::getIntRectAroundOrigin(origin, dimensions.x, dimensions.y);
	for (auto position : positions)
	{
		if (!map.contains(position.x, position.y)) continue;

		double x_drop = spatial::distance(origin, { position.x, origin.y }) * dropoff.x;
		double y_drop = spatial::distance(origin, { origin.x, position.y }) * dropoff.y;
		double avg_drop = (x_drop + y_drop) / 2.0;

		map.at(position.x, position.y) += val - avg_drop;
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

