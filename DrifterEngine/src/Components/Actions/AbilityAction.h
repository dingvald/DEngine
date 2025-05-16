#pragma once
#include <Ability/AbilityIds.h>
#include <Spatial/TilePosition.h>

struct AbilityAction
{
	entt::id_type ability = AbilityIds::NullAbility;
	std::optional<drft::TilePosition> target = std::nullopt;
};