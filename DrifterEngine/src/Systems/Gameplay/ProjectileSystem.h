#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class ProjectileSystem : public System
	{
	public:
		using System::System;

		void init() override;
		void update(const float dt) override;

	private:
		void onProjectileAdded(entt::registry& registry, entt::entity entity);
		void onProjectileRemoved(entt::registry& registry, entt::entity entity);
		void onCollisionComponentAdded(entt::registry& registry, entt::entity entity);

	private:
		std::unordered_set<entt::entity> _attackerAdded;
	};
}

