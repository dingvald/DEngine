#include "pch.h"
#include "SpendActionPoints.h"
#include "Components/Components.h"

void drft::system::spendActionPoints(int cost, ActionType type, entt::handle entity)
{
	float actionCost = cost;
	if (const auto& actorComp = entity.try_get<component::Actor>())
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
	entity.emplace_or_replace<component::action::SpendPoints>(static_cast<int>(actionCost));
}
