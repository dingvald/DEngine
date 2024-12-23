#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class StaminaSystem : public System
	{
	public:
		using System::System;
	
		void init() override;
		void updateEnd() override;

	private:
		void onStaminaAdded(entt::registry& registry, entt::entity entity) const;
		void onStaminaConsumed(entt::registry& registry, entt::entity entity) const;
		void onMoveActionAdded(entt::registry& registry, entt::entity entity) const;
	};
}


