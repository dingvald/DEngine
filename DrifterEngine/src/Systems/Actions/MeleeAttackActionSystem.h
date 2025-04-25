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
		void update() override;

		void onTweenReachedTarget(entt::handle entity, MeleeAttackAction action) const;
		void onTweenReturnedToStart(entt::handle entity) const;

		entt::id_type getEffectTexture(const std::unordered_map<std::string, int>& damageTypes) const;
	};
}


