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
		void onUpdateLate(const float dt) override;
		void onUpdateEnd() override;

		void onMeleeAttackActionAdded(entt::registry& registry, entt::entity entity) const;
		void processMeleeAttackAction(entt::entity entity, MeleeAttackAction action) const;
		entt::id_type getEffectTexture(const std::unordered_map<std::string, int>& damageTypes) const;
	};
}


