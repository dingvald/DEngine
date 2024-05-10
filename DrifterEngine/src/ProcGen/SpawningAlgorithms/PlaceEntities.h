#pragma once

namespace drft::gen
{
	void placeMany(const std::string& name, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg);
	void placeMany(entt::entity prototype, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg);
}