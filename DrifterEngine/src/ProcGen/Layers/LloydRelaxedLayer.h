#pragma once
#include <JSON/ICreateFromJson.h>
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

namespace drft
{
	class LloydRelaxedLayer;

	class LloydRelaxedLayerChunk : public GenerationChunk<LloydRelaxedLayer, LloydRelaxedLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(int level) override;

	private:
		virtual int numLevels() const override { return 3; }
		GenerationState generateRandomPoints(spatial::AABB<int> volume);
		GenerationState collectNeighborPoints(spatial::AABB<int> volume);
		GenerationState applyRelaxationToPoints(spatial::AABB<int> volume);

	public:
		std::unordered_set<sf::Vector2i> randomPoints;
		std::unordered_set<sf::Vector2i> neighborPoints;
		std::unordered_set<sf::Vector2i> distributedPoints;
	};

	class LloydRelaxedLayer : public GenerationLayer<LloydRelaxedLayer, LloydRelaxedLayerChunk>, public IGetValueAtLayer
	{
	public:
		LloydRelaxedLayer();

		double getValueAt(sf::Vector3i position) override;
		void createFromJson(const rapidjson::Value& json) override;

		float getDistributionDensity() const;
		int getNumberOfRelaxationPasses() const;

	private:
		float _density = 0.5;
		float _passes = 1;
	};
}

