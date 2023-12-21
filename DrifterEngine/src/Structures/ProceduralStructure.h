#pragma once
#include "StructureBase.h"

namespace drft
{
	class ProceduralStructure : public StructureBase
	{
	public:
		virtual sf::IntRect stamp(sf::Vector2i origin, entt::registry& registry) override;

	private:
		struct EntityProbabilityPair
		{
			entt::entity entity = entt::null;
			float probability = 0.0f;
		};
		
		std::unordered_map<std::string, std::vector<EntityProbabilityPair>> _entityCategories;
		std::unordered_map<std::string, float> _parameters;
		std::function<
	};
}


