#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class BodyPartSystem : public System
	{
	public:
		void init() override;
		void update(const float dt) override;

	private:
		void onIncomingDamage(entt::registry& registry, entt::entity entity);

		int calculateDamageFromHeld(entt::entity attacker, unsigned long itemR, unsigned long itemL);
		int calculateMitigationFromWorn(entt::entity defender, unsigned long partHit);
		std::string determinePartHit(std::unordered_map<std::string, unsigned long>& parts);
	};

}


