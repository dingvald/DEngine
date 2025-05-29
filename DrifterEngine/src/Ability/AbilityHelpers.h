#pragma once
#include <Spatial/TilePosition.h>

namespace drft
{
	class IAbility;
}

namespace AbilityHelpers
{
	void performAbility(entt::handle actor, const drft::IAbility& ability, std::optional<drft::TilePosition> target);
	void performTargetedAbility(entt::handle actor, const drft::IAbility& ability);
	void performDirectionalAbility(entt::handle actor, const drft::IAbility& ability);

	bool hasResources(entt::const_handle actor, const drft::IAbility& ability);
	void spendResources(entt::handle actor, const drft::IAbility& ability);
}