#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class LevelingSystem : public System
	{
	public:
		using System::System;

		void init() override;
		void onUpdateEnd() override;

	private:
		void onXPGained(entt::registry& registry, entt::entity entity);
		void onLevelUp(entt::registry& registry, entt::entity entity);
	};
}


