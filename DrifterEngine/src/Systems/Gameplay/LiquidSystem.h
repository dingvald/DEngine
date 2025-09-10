#pragma once
#include "Systems/System.h"
#include "Events/TurnEndEvent.h"
#include "Utility/stdHashing.h"

namespace drft::spatial
{
	class WorldGrid;
}

namespace drft
{
	struct TilePosition;
}

namespace drft::system
{
	class LiquidSystem : public System
	{
	public:
		using System::System;

		void init() override;
		void update(const float dt) override;

	private:
		void addInLiquidEffect(sf::Vector3i position, sf::Color color);
		entt::entity getLiquidAt(TilePosition tilePosition) const;

		void onTurnEndEvent(events::TurnEndEvent& ev) const;

		void onUpdateInLiquid(entt::registry& registry, entt::entity entity);
		void onRemoveInLiquid(entt::registry& registry, entt::entity entity);

	private:
		spatial::WorldGrid* _grid = nullptr;
		std::vector<entt::entity> _inLiquidEffects;
	};
}


