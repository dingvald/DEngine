#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

namespace drft
{
	class LloydRelaxedLayer;

	class LloydRelaxedLayerChunk : public GenerationChunk<LloydRelaxedLayer, LloydRelaxedLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(GenerationLevel desiredLevel) override;

	private:
		virtual GenerationLevel numLevels() const override { return GenerationLevel::Three; }
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
		using GenerationLayer::GenerationLayer;

		double getValueAt(sf::Vector3i position) override;
		void createFromJson(const rapidjson::Value& json) override;

		float getDistributionDensity() const;
		int getNumberOfRelaxationPasses() const;

		sf::Vector3i getChunkDimensions() const override;

	private:
		float _density = 0.5;
		float _passes = 1;
	};
}

