#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

#include <JSON/ICreateFromJson.h>

namespace drft
{
	class BiomeLayer;

	using BiomeCentroids = std::unordered_map<sf::Vector2i, const Biome*>;
	using GeneratedDependencies = std::unordered_map<entt::id_type, IGetValueAtLayer*>;

	struct BiomeSlotPoint
	{
		const Biome* biome;
		entt::id_type slotID;
		sf::Vector2i point;
	};

	class BiomeLayerChunk : public GenerationChunk<BiomeLayer, BiomeLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(GenerationLevel desiredLevel) override;

	private:
		GenerationState assignBiomesToVoronoiCells(spatial::AABB<int> volume);
		void assignBiomeToVoronoiCell(sf::Vector3i centroid, const SlotDeterminer::DependencyValues& dependencyValues);
		SlotDeterminer::DependencyValues getDependencyValuesAtPoint(sf::Vector3i point, const GeneratedDependencies& generatedDependencies) const;
		virtual GenerationLevel numLevels() const override { return GenerationLevel::One; }

	public:
		BiomeCentroids biomePoints;
	};

	class BiomeLayer : public GenerationLayer<BiomeLayer, BiomeLayerChunk>, public ICreateFromJson
	{
	public:
		using GenerationLayer::GenerationLayer;

		void createFromJson(const rapidjson::Value& json) override;

		sf::Vector3i getChunkDimensions() const override;

		const std::vector<const Biome*>& getBiomes() const;
		const std::unordered_set<entt::id_type>& getLayerDependencies() const;
		bool satisfiesBiomeSpecificDependencies(const Biome& biome, const SlotDeterminer::DependencyValues& dependencyValues) const;
		// Gets the Biome at a given tile positions
		const Biome* getBiomeAt(sf::Vector3i tilePosition) const;
		void forEachBiomeInArea(sf::IntRect area, int z, std::function<void(const Biome* biome)> func);

	private:
		std::vector<const Biome*> _biomes;
		std::unordered_set<entt::id_type> _layerDependencies;
		std::unordered_map<std::string, SlotDeterminer> _biomeSpecificLayerDependencies;
	};
}

