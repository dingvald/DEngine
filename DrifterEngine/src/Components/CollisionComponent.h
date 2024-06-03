#pragma once
#include <EnTT/entity/entity.hpp>

struct CollisionComponent
{
	sf::Vector2i direction;
	std::vector<entt::entity> blockers;
};
