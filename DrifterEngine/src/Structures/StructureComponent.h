#pragma once

namespace drft
{
	using PositionList = std::vector<sf::Vector2i>;
	class StructureComponent
	{
	public:
		virtual ~StructureComponent() = default;
		virtual void createFromJSON(const rapidjson::Value& json) = 0;
		virtual void apply(std::unordered_map<entt::entity, PositionList>& layout) const = 0;
	};
}


