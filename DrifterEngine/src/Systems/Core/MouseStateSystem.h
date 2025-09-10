#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class MouseStateSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void update(const float dt) override;

		void updateMousePosition(entt::entity mouse) const;
		void updateMouseContextualState(entt::entity mouse) const;
	};

}