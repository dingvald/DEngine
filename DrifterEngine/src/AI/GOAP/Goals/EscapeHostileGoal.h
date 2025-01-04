#pragma once
#include "AI/GOAP/IGoal.h"

namespace drft::goap
{
	class EscapeHostileGoal : public IGoal
	{
	public:
		EscapeHostileGoal();
		virtual float utility(entt::const_handle agent) const override;
	};
}


