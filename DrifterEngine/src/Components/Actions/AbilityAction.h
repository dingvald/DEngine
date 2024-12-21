#pragma once
#include <Ability/AbilityType.h>
#include <Spatial/TilePosition.h>

struct AbilityAction
{
	drft::AbilityType ability = drft::AbilityType::NullAbility;
	std::optional<drft::TilePosition> target = std::nullopt;
};