#pragma once

namespace drft
{
	using PositionList = std::vector<sf::Vector2i>;
	class StructureComponent
	{
	public:
		virtual void apply(std::unordered_map<entt::entity, PositionList>& layout) const = 0;
	};
}


