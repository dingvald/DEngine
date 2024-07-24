#pragma once
#include "Systems/System.h"

namespace drft::events
{
	struct EntityConsumeEvent;
}

namespace drft::system
{
	class HealingSystem : public System
	{
	public:
		using System::System;

		void init() override;

	private:
		void onConstructConsumeEntity(entt::registry& registry, entt::entity entity) const;
	};
}

