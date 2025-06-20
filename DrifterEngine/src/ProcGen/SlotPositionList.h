#pragma once
#include <SFML/System/Vector3.hpp>
#include <vector>
#include <ProcGen/EntityPack/NullEntitySlot.h>
#include <ProcGen/Layers/CanvasLayer.h>

struct SlotPositionPair
{
	entt::id_type slot = NullEntitySlot;
	sf::Vector3i position = {0,0,0};
	int priority = CanvasLayer::UninitializedPriority;
};

using SlotPositionList = std::vector<SlotPositionPair>;