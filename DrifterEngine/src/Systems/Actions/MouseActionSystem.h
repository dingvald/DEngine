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
		void update(const float dt) override;

		void onMouseContextActionAdded(entt::registry& registry, entt::entity entity) const;
		void onMouseInspectActionAdded(entt::registry& registry, entt::entity entity) const;
	};
}
