#pragma once
#include "Systems/System.h"

namespace drft::spatial
{
	class WorldGrid;
}

namespace drft::system
{
	class InteractionSystem : public System
	{
	private:
		void init() override;
		void update(const float dt) override;
		void onUpdateEnd() override;

		std::vector<entt::entity> getInteractableSurroundings(sf::Vector2i position, const spatial::WorldGrid& grid);
		bool onTargetSelected(entt::entity actor, sf::Vector2i target);
		void onContructDoInteract(entt::registry& registry, entt::entity entity);
	};
}


