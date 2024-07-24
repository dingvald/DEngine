#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class DetermineCraftableItemsSystem : public System
	{
	public:
		using System::System;

		void init() override;

	private:
		void onContainerUpdated(entt::registry& registry, entt::entity entity);
	};

}

