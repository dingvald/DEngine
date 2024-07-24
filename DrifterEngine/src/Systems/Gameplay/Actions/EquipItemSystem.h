#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class EquipItemSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void onUpdateEnd() override;

		void onItemEquipped(entt::registry& registry, entt::entity entity);
		void onItemUnequipped(entt::registry& registry, entt::entity entity);
	};

}


