#pragma once
#include "Systems/System.h"
#include "Events/GameTickEvent.h"
#include "Events/EnterTileEvent.h"
#include "Events/LeaveTileEvent.h"
#include "Events/TurnEndEvent.h"

namespace drft::spatial
{
	class WorldGrid;
}

namespace drft::system
{
	class LiquidSystem : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;
		void onFixedUpdateEnd() override;

	private:
		bool isAffectedByLiquids(entt::entity entity) const;
		void addInLiquidEffect(sf::Vector2f position, sf::Color color);

		void onEnterTileEvent(events::EnterTileEvent& ev) const;
		void onTurnEndEvent(events::TurnEndEvent& ev) const;
		void onUpdateInLiquid(entt::registry& registry, entt::entity entity);
		void onRemoveInLiquid(entt::registry& registry, entt::entity entity);

	private:
		spatial::WorldGrid* _grid;
		std::vector<entt::entity> _inLiquidEffects;
	};
}


