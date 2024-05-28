#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class OpenCraftingSystem :public System
	{
	private:
		void init() override;

		void onOpenCraftingAction(entt::registry& registry, entt::entity entity) const;
	};

}


