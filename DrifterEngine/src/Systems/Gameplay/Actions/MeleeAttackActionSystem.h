#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class MeleeAttackActionSystem : public System
	{
	private:
		void init() override;
		void onUpdateEnd() override;

		void onPerformMeleeAttackAction(entt::registry& registry, entt::entity entity) const;
		void onTryMeleeAttackAction(entt::registry& registry, entt::entity entity) const;
		void onDoMeleeAttackAction(entt::registry& registry, entt::entity entity) const;
	};
}


