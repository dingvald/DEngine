#pragma once

namespace drft::util
{
	// Create a number of entities in the given registry with a random position in the given area
	void buildMany(std::string entity, int number, sf::IntRect inArea, entt::registry& registry);
}