#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class DetermineCraftableItemsSystem : public System
	{
	public:
		void init() override;
		void update(const float dt) override;

	private:
		void onContainerUpdated(entt::registry& registry, entt::entity entity);
	};

}

