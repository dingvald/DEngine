#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class DropItemSystem : public System
	{
	public:
		void init() override;
		void update(const float dt) override;
		void onUpdateEnd() override;

	private:
		void onItemDropped(entt::registry&, entt::entity);
	};
}


