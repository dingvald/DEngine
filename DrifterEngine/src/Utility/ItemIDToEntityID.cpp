#include "pch.h"
#include "ItemIDToEntityID.h"

entt::entity drft::util::ItemIDToEntityID(component::Item::ID id, const entt::registry& registry)
{
	entt::entity result = entt::null;
	auto view = registry.view<component::Item>();
	for (auto [entity, item] : view.each())
	{
		if (item.id == id)
		{
			result = entity;
			break;
		}
	}

	return result;
}
