#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class TargetSelectSystem : public System
	{
		void init() override;
		void onTargetSelect(entt::registry& registry, entt::entity entity);
	};
}

