#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

namespace drft
{
	class EntityFactory;
}

class GenerationLayerManager;

namespace drft::gen
{
	struct GenerationContext;

	class WorldGenerator
	{
	public:
		WorldGenerator();
		void init();
		void createFromJson(const std::string& JSONfilename);

		void generate();

		GenerationState generateChunk(sf::Vector2i coordinate, entt::registry& registry);
		sf::Vector2i getDimensions() const;

		template<class Archive>
		void load(Archive& iarchive);
		template<class Archive>
		void save(Archive& oarchive) const;


	private:
		unsigned int _seed = 0;
		sf::Vector2i _dimensions;
		std::unique_ptr<GenerationLayerManager> _layerManager;
	};

	template<class Archive>
	inline void WorldGenerator::load(Archive& archive)
	{
		archive(cereal::make_nvp("Seed", _seed));
		archive(cereal::make_nvp("Width", _dimensions.x));
		archive(cereal::make_nvp("Height", _dimensions.y));
	}
	template<class Archive>
	inline void WorldGenerator::save(Archive& archive) const
	{
		archive(cereal::make_nvp("Seed", _seed));
		archive(cereal::make_nvp("Width", _dimensions.x));
		archive(cereal::make_nvp("Height", _dimensions.y));
	}
}


