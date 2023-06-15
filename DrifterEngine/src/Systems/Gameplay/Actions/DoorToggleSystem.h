#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class DoorToggleSystem : public System
	{
	public:
		void init() override;
		void onUpdateEnd();

	private:
		void onContructDoorToggle(entt::registry& registry, entt::entity entity);
	};
}


