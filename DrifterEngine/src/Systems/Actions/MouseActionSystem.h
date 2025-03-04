#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class MouseActionSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void update() override;

		void onMouseActionAdded(entt::registry& registry, entt::entity entity) const;
	};
}
