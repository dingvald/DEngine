#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class DropItemSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void update(const float dt) override;
		void onItemDropped(entt::registry&, entt::entity);
	};
}


