#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class SelectDirectionSystem : public System
	{
	public:
		void init() override;

	private:
		void onSelectDirection(entt::registry& registry, entt::entity entity);
	};
}


