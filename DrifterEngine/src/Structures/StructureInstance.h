#pragma once

namespace drft
{
	class StructureInstance
	{
	public:
		StructureInstance(std::unordered_map<entt::entity, std::vector<sf::Vector2i>> layout);
		void stamp(sf::Vector2i tileOrigin, entt::registry& registry) const;

	private:
		sf::Vector2i _dimensions;
		std::unordered_map<entt::entity, std::vector<sf::Vector2i>> _layout;
	};
}


