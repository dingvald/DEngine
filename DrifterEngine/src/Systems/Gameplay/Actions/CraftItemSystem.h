#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class CraftItemSystem : public System
	{
	private:
		void init() override;
		void onUpdateEnd() override;
		void onCraftItem(entt::registry& registry, entt::entity entity);
	};

}


