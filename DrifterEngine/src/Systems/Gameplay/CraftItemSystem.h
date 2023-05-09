#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class CraftItemSystem : public System
	{
	public:
		void init() override;
		void update(const float dt) override;

	private:
		void onCraftItem(entt::registry& registry, entt::entity entity);
	};

}


