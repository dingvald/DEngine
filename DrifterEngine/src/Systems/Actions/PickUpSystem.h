#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class PickUpSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void onUpdateEnd() override;

		void onPickupAction(entt::registry& registry, entt::entity entity) const;
	};
}


