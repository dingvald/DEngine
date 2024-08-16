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

		const entt::dense_set<sf::Vector2i>& getArea() const;

	private:
		sf::IntRect _area;
		std::unique_ptr<StructureShapeInstance> _shape;
	};
}


