#pragma once
#include <Components/Wrappers/ComponentWrapper.h>
#include <Components/HotbarComponent.h>
#include <Ability/AbilityIds.h>

namespace drft
{
	class HotbarWrapper : public ComponentWrapper<HotbarComponent>
	{
	public:
		void setAbility(entt::id_type abilityId, unsigned int hotbarIndex);
		entt::id_type getAbility(unsigned int hotbarIndex) const;
		void removeAbility(unsigned int hotbarIndex);
		std::optional<unsigned int> findAbilityIndex(entt::id_type abilityId) const;
	};
}