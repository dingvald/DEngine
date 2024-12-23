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
		void updateEnd() override;

	private:
		std::vector<entt::entity> getInteractableSurroundings(sf::Vector3i position, const spatial::WorldGrid& grid);
		void doInteract(entt::entity actor, const std::vector<entt::entity>& interactables) const;
		bool onTargetSelected(entt::entity actor, sf::Vector3i target);
		void onConstructInteractionAction(entt::registry& registry, entt::entity entity);
	};
}


