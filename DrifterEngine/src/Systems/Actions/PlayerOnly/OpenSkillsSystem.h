#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class OpenSkillsSystem :public System
	{
	public:
		using System::System;

	private:
		void init() override;

		void onOpenSkillsScreenAction(entt::registry& registry, entt::entity entity) const;
	};

}