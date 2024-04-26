#pragma once
#include "PerlinNoise.h"

namespace drft::rng
{
	class NoiseLayer
	{
	public:
		void init(sf::Vector2i dimensions, double resolution, PerlinNoise noise);
		using BiasingFunction = std::function<double(double, sf::Vector2i)>;
		double getValueAt(sf::Vector2i tilePosition) const;
		// Adds a function to bias the generated noise
		// Calls the functions in the order they are added
		void addBiasingFunction(BiasingFunction&& func);

	private:
		double _getValueAt(sf::Vector2i tilePosition) const;

	private:
		double _min = FLT_MAX;
		double _max = FLT_MIN;
		double _resolution = 1.0;
		sf::Vector2i _dimensions;
		PerlinNoise _noise;
		std::vector<BiasingFunction> _biasingFunctions;
	};
}


