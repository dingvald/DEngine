#pragma once
#include <SFML/System/Vector2.hpp>
#include <EnTT/entity/entity.hpp>
#include <vector>
#include <unordered_map>
#include <string>

struct MeleeAttackAction
{
	sf::Vector2i direction = { 0,0 };
	std::vector<entt::entity> targets;
	std::unordered_map<std::string, int> damageTypes;
};