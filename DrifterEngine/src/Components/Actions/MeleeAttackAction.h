#pragma once
#include <SFML/System/Vector2.hpp>


struct MeleeAttackAction
{
	sf::Vector2i direction = { 0,0 };
	entt::const_handle itemUsed = {};
};

