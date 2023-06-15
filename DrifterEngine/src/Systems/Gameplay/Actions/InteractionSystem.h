#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class InteractionSystem : public System
	{
	public:
		void init() override;
		void update(const float dt) override;
		void onUpdateEnd() override;

	private:
		void onTargetSelected(entt::entity actor, sf::Vector2i target);
		void onContructDoInteract(entt::registry& registry, entt::entity entity);

	private:
	};
}


