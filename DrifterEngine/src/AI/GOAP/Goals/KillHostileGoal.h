#pragma once
#include "AI/GOAP/IGoal.h"

namespace drft::goap
{
	class KillHostileGoal : public IGoal
	{
	public:
		KillHostileGoal();
		virtual float utility(entt::const_handle agent) const override;
	};
}


