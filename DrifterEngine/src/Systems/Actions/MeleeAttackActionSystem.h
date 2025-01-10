#pragma once
#include "Systems/System.h"

struct MeleeAttackAction;

namespace drft::system
{
	class MeleeAttackActionSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void update() override;

		void onMeleeAttackActionAdded(entt::registry& registry, entt::entity entity) const;

		void onCollideWithTarget(entt::handle entity, MeleeAttackAction action) const;
		void onReturnToStartPosition(entt::handle entity) const;

		entt::id_type getEffectTexture(const std::unordered_map<std::string, int>& damageTypes) const;
	};
}


