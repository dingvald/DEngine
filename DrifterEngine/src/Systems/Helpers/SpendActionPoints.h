#pragma once

namespace drft::system
{
	static constexpr int BASE_ACTION_COST = 100;

	enum class ActionType
	{
		Move,
		Act
	};

	void spendActionPoints(int cost, ActionType type, entt::handle entity);
}