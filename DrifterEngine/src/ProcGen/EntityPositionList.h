#pragma once
#include <SFML/System/Vector3.hpp>
#include <vector>

struct EntityPositionPair
{
	entt::id_type slot;
	sf::Vector3i position;
};

using EntityPositionList = std::vector<EntityPositionPair>;