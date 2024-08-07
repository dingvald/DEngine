#pragma once
#include <ProcGen/GenerationLayer/AbstractLayer.h>
#include <ProcGen/GenerationLayer/GenerationLayerDependency.h>
#include <Utility/stdHashing.h>
#include <EnTT/container/dense_map.hpp>
#include <vector>

template<typename Layer, typename Chunk>
class GenerationLayer : public AbstractLayer
{
public:
	GenerationState generate(GenerationContext&& context) override final;

	virtual sf::Vector2i dimensions() const { return { 64, 64 }; };

protected:
	void addDependency(GenerationLayerDependency dependency);

	Chunk* tryGetChunk(sf::Vector2i tilePosition);

	sf::Vector2i toChunkPosition(sf::Vector2i tilePosition);
	sf::Vector2i toChunkLocalPosition(sf::Vector2i tilePosition);
	std::vector<sf::Vector2i> getChunkPointsInsideArea(sf::IntRect area);

private:
	bool checkIfChunksLoaded(const std::vector<sf::Vector2i>& chunks) const;
	bool generateDependencies(GenerationContext& context);
	bool generateChunks(const std::vector<sf::Vector2i>& chunks, GenerationContext& context);

private:
	entt::dense_map<sf::Vector2i, Chunk> _chunks;
	std::vector<GenerationLayerDependency> _dependencies;

};

#include <ProcGen/GenerationLayer/GenerationLayer.inl>