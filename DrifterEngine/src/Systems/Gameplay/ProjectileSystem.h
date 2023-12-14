#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class ProjectileSystem : public System
	{
		void init() override;
		void update(float dt) override;

		void onProjectileAdded(entt::registry& registry, entt::entity);
		void onProjectileRemoved(entt::registry& registry, entt::entity);

	private:
		std::unordered_set<entt::entity> _attackerAdded;
	};
}

