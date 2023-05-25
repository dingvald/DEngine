#include "pch.h"
#include "EntityHelpers.h"
#include "Components/Components.h"

std::string drft::util::getEntityName(const entt::const_handle entity)
{
	if (auto info = entity.try_get<component::Info>())
	{
		return info->name;
	}
	return "UNNAMED";
}