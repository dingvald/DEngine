#pragma once
#include <SFML/System/Vector2.hpp>
#include <EnTT/entity/entity.hpp>
#include <vector>
#include <unordered_map>
#include <string>

// Add to entity to perform the action
struct PerformMeleeAttackAction
{
	sf::Vector2i direction = { 0,0 };
	std::vector<entt::entity> targets;
};

// Should only be reacted to and/or modified (except for "owning" system)
struct TryMeleeAttackAction
{
	sf::Vector2i direction = { 0,0 };
	std::vector<entt::entity> targets;
	std::unordered_map<std::string, int> damageTypes;
};

// Should only be reacted to and/or modified (except for "owning" system)
struct DoMeleeAttackAction
{
	sf::Vector2i direction;
	std::vector<entt::entity> targets;
	std::unordered_map<std::string, int> damageTypes;
};