#pragma once
#include "Components/Components.h"

// TODO: could cache item entity mappings as a static unordered map in the Item ID Generator system

namespace drft::util
{
	entt::entity ItemIDToEntityID(component::Item::ID id, const entt::registry& registry);
}

