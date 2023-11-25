#include "pch.h"
#include "EntityHelpers.h"
#include "Components/Components.h"
#include "Spatial/Helpers.h"

std::string drft::util::getEntityName(const entt::const_handle entity)
{
	if (auto info = entity.try_get<component::Info>())
	{
		return info->name;
	}
	return "UNNAMED";
}

int drft::util::getDistanceBetween(entt::const_handle e1, entt::const_handle e2)
{
	if (auto pos1 = e1.try_get<component::Position>())
	{
		if (auto pos2 = e2.try_get<component::Position>())
		{
			return spatial::distance(pos1->position, pos2->position);
		}
	}
	return 0;
}
