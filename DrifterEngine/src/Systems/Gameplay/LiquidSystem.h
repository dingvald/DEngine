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
		void addInLiquidEffect(sf::Vector2i position, sf::Color color);

		void onTurnEndEvent(events::TurnEndEvent& ev) const;

		void onUpdateInLiquid(entt::registry& registry, entt::entity entity);
		void onRemoveInLiquid(entt::registry& registry, entt::entity entity);

	private:
		spatial::WorldGrid* _grid = nullptr;
		entt::dense_map<sf::Vector2i, entt::entity> _liquidPositions;
		std::vector<entt::entity> _inLiquidEffects;
	};
}


