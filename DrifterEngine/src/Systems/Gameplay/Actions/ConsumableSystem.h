#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class ConsumableSystem : public System
	{
	public:
		void init() override;

	private:
		void consumeInteraction(entt::entity actor, entt::entity subject) const;

		void onConstructConsumable(entt::registry& registry, entt::entity entity) const;
		void onDestroyConsumable(entt::registry& registry, entt::entity entity) const;
	};
}

