#pragma once
#include <SFML/System/Vector3.hpp>
#include <vector>
#include <ProcGen/EntityPack/NullEntitySlot.h>
#include <Utility/stdHashing.h>

static const int DEFAULT_SLOT_PRIORITY = 0;
static const int UNINITIALIZED_SLOT_PRIORITY = std::numeric_limits<int>::min();

struct SlotPriority
{
	entt::id_type slot = NullEntitySlot;
	int priority = UNINITIALIZED_SLOT_PRIORITY;
};

struct SlotPositionPair
{
	entt::id_type slot = NullEntitySlot;
	sf::Vector3i position = {0,0,0};
	int priority = UNINITIALIZED_SLOT_PRIORITY;
};

using SlotPositionList = std::vector<SlotPositionPair>;
using SlotPositionMap = std::unordered_map<sf::Vector3i, SlotPriority>;

void mergeSlotPositionMaps(SlotPositionMap& map, const SlotPositionMap& other);
void mergeSlotPositionMaps(SlotPositionMap& map, const SlotPositionMap& other, std::function<bool(sf::Vector3i)> pred);
void mergeSlotListIntoMap(SlotPositionMap& map, const SlotPositionList& list);