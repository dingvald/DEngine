#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include "ItemComponent.h"

struct InteractableComponent
{
	std::unordered_map<std::string, std::function<void(entt::entity, entt::entity)>> interactions;
};



