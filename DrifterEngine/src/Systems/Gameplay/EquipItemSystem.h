#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class EquipItemSystem : public System
	{
	public:
		void init() override;
		void onUpdateEnd() override;

	private:
		void onItemEquipped(entt::registry& registry, entt::entity entity);
		void onItemUnequipped(entt::registry& registry, entt::entity entity);
	};

}


