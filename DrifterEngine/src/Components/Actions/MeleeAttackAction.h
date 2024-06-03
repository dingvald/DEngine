#pragma once
#include <SFML/System/Vector2.hpp>
#include <EnTT/entity/entity.hpp>
#include <vector>
#include <unordered_map>
#include <string>

struct PerformMeleeAttackAction
{
	sf::Vector2i direction = { 0,0 };
	std::vector<entt::entity> targets;
};

struct TryMeleeAttackAction
{
	sf::Vector2i direction = { 0,0 };
	std::vector<entt::entity> targets;
	std::unordered_map<std::string, int> damageTypes;

	bool cancel = false;
};

struct DoMeleeAttackAction
{
	sf::Vector2i direction;
	std::vector<entt::entity> targets;
	std::unordered_map<std::string, int> damageTypes;
};