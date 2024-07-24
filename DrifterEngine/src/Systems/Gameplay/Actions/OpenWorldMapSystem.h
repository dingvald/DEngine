#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class OpenWorldMapSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		
		void onOpenWorldMapAction(entt::registry& registry, entt::entity entity) const;
	};
}


