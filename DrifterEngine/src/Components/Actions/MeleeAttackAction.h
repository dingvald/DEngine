#pragma once
#include <SFML/System/Vector2.hpp>
#include <Systems/Helpers/DamageTypes.h>


struct MeleeAttackAction
{
	sf::Vector2i direction = { 0,0 };
	Damage::TypeMap damageTypes = {};
};

