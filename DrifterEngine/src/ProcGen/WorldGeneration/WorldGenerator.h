#pragma once
#include <JSON/ICreateFromJson.h>
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <Random/Random.h>
#include <Spatial/ChunkPosition.h>

namespace drft
{
	class EntityFactory;
}

class GenerationLayerManager;

namespace drft::gen
{
	struct GenerationContext;

	class WorldGenerator : public ICreateFromJson
	{
	public:
		void init();
		void createFromJson(const rapidjson::Value& json) override;

		void generate();

		GenerationState generateChunk(ChunkPosition coordinate, entt::registry& registry);
		ChunkPosition getDimensions() const;

		template<class Archive>
		void load(Archive& iarchive);
		template<class Archive>
		void save(Archive& oarchive) const;


	private:
		unsigned int _seed = 0;
		ChunkPosition _dimensions;
		std::unique_ptr<GenerationLayerManager> _layerManager;
	};

	template<class Archive>
	inline void WorldGenerator::load(Archive& archive)
	{
		archive(cereal::make_nvp("Seed", _seed));
		archive(cereal::make_nvp("Width", _dimensions.x));
		archive(cereal::make_nvp("Height", _dimensions.y));
		rng::GlobalSeed = _seed;
	}
	template<class Archive>
	inline void WorldGenerator::save(Archive& archive) const
	{
		archive(cereal::make_nvp("Seed", _seed));
		archive(cereal::make_nvp("Width", _dimensions.x));
		archive(cereal::make_nvp("Height", _dimensions.y));
	}
}


