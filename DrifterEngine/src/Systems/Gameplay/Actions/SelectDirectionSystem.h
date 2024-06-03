#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class SelectDirectionSystem : public System
	{
	private:
		void init() override;

		void onSelectDirection(entt::registry& registry, entt::entity entity);
	};
}


