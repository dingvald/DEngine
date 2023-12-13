#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class ProjectileSystem : public System
	{
		void init() override;
		void update(float dt) override;

		void onProjectileAdded(entt::registry& registry, entt::entity);
	};
}

