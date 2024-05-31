#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class StaminaSystem : public System
	{
	private:
		void init() override;
		void onUpdateEnd() override;

		void onStaminaAdded(entt::registry& registry, entt::entity entity) const;
		void onStaminaConsumed(entt::registry& registry, entt::entity entity) const;
		void onDoMoveAction(entt::registry& registry, entt::entity entity) const;
	};
}


