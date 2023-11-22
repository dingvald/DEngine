#pragma once
#include "IAction.h"

namespace drft::goap
{
	class SpotHostileAction : public IAction
	{
	public:
		SpotHostileAction();

		bool setTarget(entt::const_handle agent) const override;
		bool perform(entt::handle agent, entt::handle target) const override;
		int cost() const override;
		bool requiresInRange() const override;
	};
}

