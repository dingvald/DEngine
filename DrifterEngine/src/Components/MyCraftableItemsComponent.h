#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include "ItemComponent.h"

struct MyCraftableItemsComponent
{
	std::vector<entt::entity> craftables;
	std::vector<entt::entity> partialCraftables;
};


