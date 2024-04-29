#pragma once
#include "PerlinNoise.h"

namespace drft::rng
{
	class NoiseLayer
	{
	public:
		NoiseLayer() = default;
		NoiseLayer(sf::Vector2i dimensions, unsigned int seed);
		using BiasingFunction = std::function<double(double, sf::Vector2i)>;
		double getValueAt(sf::Vector2i tilePosition) const;
		// Adds a function to bias the generated noise
		// Calls the functions in the order they are added
		void addBiasingFunction(BiasingFunction&& func);

		void createFromJSON(const rapidjson::Value& json);
		template<class Archive>
		void load(Archive& iarchive);
		template<class Archive>
		void save(Archive& iarchive) const;

	private:
		double _getValueAt(sf::Vector2i tilePosition) const;

	private:
		double _min = FLT_MAX;
		double _max = FLT_MIN;
		double _resolution = 1.0;
		sf::Vector2i _dimensions;
		int _octaves = 8;
		float _lacunarity = 2.0f;
		float _gain = 0.5f;
		PerlinNoise _noise;
		unsigned int _seed;
		std::vector<BiasingFunction> _biasingFunctions;
	};


	template<class Archive>
	inline void rng::NoiseLayer::load(Archive& archive)
	{
		archive(_min, _max, _resolution, _dimensions.x, _dimensions.y, _octaves, _lacunarity, _gain, _seed);
		_noise = rng::PerlinNoise{ _seed, _octaves, _lacunarity, _gain };
	}
	template<class Archive>
	inline void rng::NoiseLayer::save(Archive& archive) const
	{
		archive(_min, _max, _resolution, _dimensions.x, _dimensions.y, _octaves, _lacunarity, _gain, _seed);
	}
}


