#pragma once

namespace drft
{
	class EntityFactory;
}

namespace drft::gen
{
	entt::handle placeSingle(const std::string& name, sf::Vector2i position, entt::registry& registry, const EntityFactory& factory);

	void placeManyConditional(const std::string& name, sf::IntRect area, entt::registry& registry, std::function<bool(sf::Vector2i)> predicate);

	void placeMany(const std::string& name, sf::IntRect area, entt::registry& registry);

	void placeMany(const std::string& name, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg);

	void placeMany(const std::string& name, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg, std::function<void(entt::handle, sf::Vector2i)> onPlaceFunc);
}