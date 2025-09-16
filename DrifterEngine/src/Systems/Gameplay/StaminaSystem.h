#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class StaminaSystem : public System
	{
	public:
		using System::System;
	
		void init() override;
		void start() override;
		void updateEnd(const float dt) override;

	private:
		void setupStaminaComponentOnStart(entt::handle entity) const;
		void onStaminaComponentAdded(entt::registry& registry, entt::entity entity) const;
		void onStaminaConsumed(entt::registry& registry, entt::entity entity) const;
		void onMoveActionAdded(entt::registry& registry, entt::entity entity) const;

	private:
		entt::observer _observer;
	};
}


