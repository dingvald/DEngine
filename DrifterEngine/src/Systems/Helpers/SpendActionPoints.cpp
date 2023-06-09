#include "pch.h"
#include "SpendActionPoints.h"
#include "Components/Components.h"

void drft::system::spendActionPoints(entt::registry& registry, entt::entity actor, ActionType type)
{
	float actionCost = BASE_ACTION_COST;
	const auto actorComp = registry.try_get<component::Actor>(actor);
	if (actorComp)
	{
		switch (type)
		{
		case ActionType::Move:
			actionCost *= (1.0f / actorComp->moveSpeed);
			break;
		case ActionType::Act:
			actionCost *= (1.0f / actorComp->actSpeed);
			break;
		}
	}
	registry.emplace_or_replace<component::action::SpendPoints>(actor, static_cast<int>(actionCost));
}
