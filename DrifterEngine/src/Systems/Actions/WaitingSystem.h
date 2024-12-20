#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class WaitingSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void onUpdateEnd() override;

		void onWaitAction(entt::registry& registry, entt::entity entity) const;
	};
}

