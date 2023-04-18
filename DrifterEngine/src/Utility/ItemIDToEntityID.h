#pragma once
#include "Components/Components.h"

namespace drft::util
{
	entt::entity ItemIDToEntityID(component::Item::ID id, const entt::registry& registry);
}

