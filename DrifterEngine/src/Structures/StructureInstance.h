#pragma once

namespace drft
{
	namespace gen
	{
		struct GenerationContext;
	}

	class StructureInstance
	{
	public:
		StructureInstance(std::unordered_map<std::string, std::vector<sf::Vector2i>> layout);
		void stamp(sf::Vector2i tileOrigin, const gen::GenerationContext& context, entt::registry& registry) const;

	private:
		sf::Vector2i _dimensions;
		std::unordered_map<std::string, std::vector<sf::Vector2i>> _layout;
	};
}


