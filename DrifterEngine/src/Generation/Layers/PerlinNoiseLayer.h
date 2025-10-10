#pragma once
#include <Generation/LayeredProcGen/LayeredProcGen.h>
#include <JSON/ICreateFromJson.h>
#include <Random/PerlinNoise.h>

namespace drft
{
	class PerlinNoiseLayer : public OnDemandLayer
	{
	public:
		using OnDemandLayer::OnDemandLayer;

		double getValueAt(sf::Vector3i tilePosition) override;
		void createFromJson(const rapidjson::Value& json) override;

	private:
		double _getValueAt(sf::Vector3i tilePosition) const;

	private:
		double _resolution = 1.0;
		sf::Vector2i _dimensions = { 1024, 1024 };
		int _octaves = 8;
		float _lacunarity = 2.0f;
		float _gain = 0.5f;
		bool _isNoiseInitialized = false;
		drft::rng::PerlinNoise _noise;
	};
}
