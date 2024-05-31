#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class MotionSystem : public System
	{
	private:
		void init() override;
		void onUpdateEnd() override;

		void onMotionAdded(entt::registry& registry, entt::entity entity) const;
	};
}

