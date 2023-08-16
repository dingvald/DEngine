#pragma once
#include "Spatial/Grid.h"

namespace drft::gen
{
	void setCircle(sf::Vector2i origin, int radius, double val, float dropoff, spatial::Grid<double>& map);
	void setDropOffCircle(sf::Vector2i origin, double val, float dropoff, spatial::Grid<double>& map);
	void setDropOffRect(sf::IntRect area, double val, float dropoff, spatial::Grid<double>& map);
	void remap(double iMin, double iMax, double oMin, double oMax, spatial::Grid<double>& map);
	void customShaper(spatial::Grid<double>& map, std::function<void(double&, sf::Vector2i)> shaper);
}