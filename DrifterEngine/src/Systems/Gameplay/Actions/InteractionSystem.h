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
	public:
		using System::System;

		void init() override;
		void onUpdateEnd() override;

	private:
		std::vector<entt::entity> getInteractableSurroundings(sf::Vector2i position, const spatial::WorldGrid& grid);
		void doInteract(entt::entity actor, const std::vector<entt::entity>& interactables) const;
		bool onTargetSelected(entt::entity actor, sf::Vector2i target);
		void onConstructInteractionAction(entt::registry& registry, entt::entity entity);
	};
}


