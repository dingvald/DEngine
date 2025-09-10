#pragma once
#pragma	once
#include "Systems/System.h"


namespace drft::system
{
	class StairsMoveActionSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void update(const float dt) override;

		void onMoveUpStairsActionAdded(entt::registry& registry, entt::entity entity) const;
		void onMoveDownStairsActionAdded(entt::registry& registry, entt::entity entity) const;
	};
}