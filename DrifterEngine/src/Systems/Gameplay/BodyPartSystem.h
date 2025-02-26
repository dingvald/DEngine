#pragma once
#include "Systems/System.h"

struct BodyPart;

namespace drft::system
{
	class BodyPartSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;

		void onIncomingDamage(entt::registry& registry, entt::entity entity);
		void onMeleeAttackActionAdded(entt::registry& registry, entt::entity entity);

		std::unordered_map<std::string, int> calculateDamageTypesFromHeld(entt::entity attacker);
	};

}


