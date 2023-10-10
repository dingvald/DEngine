#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class BodyPartSystem : public System
	{
	public:
		void init() override;

	private:
		void onIncomingDamage(entt::registry& registry, entt::entity entity);
		void onLaunchAttack(entt::registry& registry, entt::entity entity);

		int calculateForceFromHeld(entt::entity attacker);
		int calculateMitigationFromWorn(entt::entity defender, unsigned long partHit);
		std::string determinePartHit(entt::handle entity);
	};

}


