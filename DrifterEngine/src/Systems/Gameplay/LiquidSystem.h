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
		static bool isAffectedByLiquids(entt::const_handle entity);
		
	private:
		void init() override;
		void fixedUpdate() override;
		void onFixedUpdateEnd() override;
		void addInLiquidEffect(sf::Vector2i position, sf::Color color);

		void onTurnEndEvent(events::TurnEndEvent& ev) const;

		void onUpdateInLiquid(entt::registry& registry, entt::entity entity);
		void onRemoveInLiquid(entt::registry& registry, entt::entity entity);

	private:
		spatial::WorldGrid* _grid = nullptr;
		std::vector<entt::entity> _inLiquidEffects;
	};
}


