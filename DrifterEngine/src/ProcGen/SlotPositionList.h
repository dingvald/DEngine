#pragma once
#include <EnTT/core/fwd.hpp>
#include <SFML/System/Vector3.hpp>
#include <vector>

struct SlotPositionPair
{
	entt::id_type slot;
	sf::Vector3i position;
};

using SlotPositionList = std::vector<SlotPositionPair>;