#pragma once
#include "Systems/System.h"
#include "Events/ItemBreakEvent.h"

struct BodyPart;

namespace drft::system
{
	class BodyPartSystem : public System
	{
	public:
		void init() override;

	private:
		void onIncomingDamage(entt::registry& registry, entt::entity entity);
		void onLaunchAttack(entt::registry& registry, entt::entity entity);
		void onItemBreakEvent(events::ItemBreakEvent& ev);

		std::unordered_map<std::string, int> calculateDamageTypesFromHeld(entt::entity attacker);
		std::unordered_map<std::string, int> calculateMitigationFromWorn(entt::entity defender, const BodyPart& partHit, const std::unordered_map<std::string, int> incomingDamageTypes);

		const BodyPart& determinePartHit(entt::handle entity);
	};

}


