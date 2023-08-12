#pragma once
#include "Spatial/Grid.h"

namespace drft::gen
{
	void setCircle(sf::Vector2i origin, int radius, double val, float dropoff, spatial::Grid<double>& map);
	void setDropOffCircle(sf::Vector2i origin, double val, float dropoff, spatial::Grid<double>& map);
	void setRect(sf::Vector2i origin, sf::Vector2i dimensions, double val, sf::Vector2f dropoff, spatial::Grid<double>& map);
}