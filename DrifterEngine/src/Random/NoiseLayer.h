#pragma once
#include "PerlinNoise.h"

namespace drft::rng
{
	class NoiseLayer
	{
	public:
		void setDimensions(sf::Vector2i dimensions);
		void setNoise(PerlinNoise noise);
		using BiasingFunction = std::function<double(double, sf::Vector2i)>;
		double at(sf::Vector2i tilePosition) const;
		// Adds a function to bias the generated noise
		// Calls the functions in the order they are added
		void addBiasingFunction(BiasingFunction&& func);

	private:
		sf::Vector2i _dimensions;
		PerlinNoise _noise;
		std::vector<BiasingFunction> _biasingFunctions;
	};
}


