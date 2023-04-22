#pragma once
namespace drft::util
{
	std::string getEntityName(const entt::handle entity);

	enum class ActionType
	{
		Move,
		Act
	};

	int getActionCost(const entt::handle entity, int baseCost, ActionType type);
}
