#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class OpenEquipmentSystem : public System
	{
	private:
		void init() override;
		
		void onOpenEquipmentAction(entt::registry& registry, entt::entity entity) const;
	};

}


