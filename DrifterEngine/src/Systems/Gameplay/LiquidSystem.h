#pragma once
#include "Systems/System.h"
#include "Events/TurnEndEvent.h"
#include "Utility/stdHashing.h"

namespace drft::spatial
{
	class WorldGrid;
}

namespace drft::system
{
	class LiquidSystem : public System
	{
	public:
		using System::System;

		void init() override;
		void onFixedUpdate() override;
		void onFixedUpdateEnd() override;

	private:
		void addInLiquidEffect(sf::Vector3i position, sf::Color color);

		void onTurnEndEvent(events::TurnEndEvent& ev) const;

		void onUpdateInLiquid(entt::registry& registry, entt::entity entity);
		void onRemoveInLiquid(entt::registry& registry, entt::entity entity);

	private:
		spatial::WorldGrid* _grid = nullptr;
		std::unordered_map<sf::Vector3i, entt::entity> _liquidPositions;
		std::vector<entt::entity> _inLiquidEffects;
	};
}


