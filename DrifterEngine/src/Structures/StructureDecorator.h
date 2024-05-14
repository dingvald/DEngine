#pragma once

namespace drft
{
	class StructureShapeInstance;

	using PositionList = std::vector<sf::Vector2i>;
	using Layout = std::unordered_map<std::string, PositionList>;
	class StructureDecorator
	{
	public:
		virtual void createFromJSON(const rapidjson::Value& json) = 0;
		virtual void apply(StructureShapeInstance& shape) const = 0;
	};
}


