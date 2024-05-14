#pragma once
#include "StructureShapeInstance.h"

namespace drft
{
	namespace gen
	{
		struct GenerationContext;
	}

	using PositionList = std::vector<sf::Vector2i>;
	using Layout = std::unordered_map<std::string, PositionList>;
	class StructureInstance
	{
	public:
		StructureInstance(std::unique_ptr<StructureShapeInstance> shape);
		void stamp(sf::Vector2i tileOrigin, gen::GenerationContext& context, entt::registry& registry) const;

	private:
		sf::Vector2i _dimensions;
		std::unique_ptr<StructureShapeInstance> _shape;
	};
}


