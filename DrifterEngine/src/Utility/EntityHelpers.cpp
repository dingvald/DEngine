#include "pch.h"
#include "EntityHelpers.h"
#include "Components/Components.h"

std::string drft::util::getEntityName(const entt::const_handle entity)
{
	auto info = entity.try_get<component::Info>();
	if (!info) return "UNNAMED";

	return info->name;
}

int drft::util::getActionCost(const entt::handle entity, int baseCost, ActionType type)
{
	int actionCost = baseCost;
	auto actor = entity.try_get<component::Actor>();
	if (actor)
	{
		switch (type)
		{
		case ActionType::Move:
			actionCost *= (1.0f / actor->moveSpeed);
			break;
		case ActionType::Act:
			actionCost *= (1.0f / actor->actSpeed);
			break;
		default:
			break;
		}	
	}
	return actionCost;
}