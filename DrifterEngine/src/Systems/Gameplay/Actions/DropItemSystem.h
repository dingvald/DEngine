#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class DropItemSystem : public System
	{
	private:
		void init() override;
		void update(const float dt) override;
		void onUpdateEnd() override;
		void onItemDropped(entt::registry&, entt::entity);
	};
}


