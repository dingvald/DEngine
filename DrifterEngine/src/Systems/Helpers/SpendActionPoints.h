#pragma once

namespace drft::system
{
	static constexpr int BASE_ACTION_COST = 100;

	enum class ActionType
	{
		Move,
		Act
	};

	void spendActionPoints(entt::registry& registry, entt::entity actor, ActionType type);
}